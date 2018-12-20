#include "AcroFormReader.h"

#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFInteger.h>

#include <string>
#include <map>

#define SAFE_DELETE( name ) if( name ) delete name; name = nullptr;


enum eFieldType{
	eBtn,
	eTx,
	eCh,
	eSig,
};

std::map<std::string, eFieldType> stoEFieldType = {
	{ "Btn", eBtn },
	{ "Tx", eTx },
	{ "Ch", eCh },
	{ "Sig", eSig },
};


static PDFObject* safeQuery( PDFParser* parser, PDFDictionary *dict, std::string name ){
	return dict->Exists( name ) ? parser->QueryDictionaryObject( dict, name ) : nullptr;
}

//Fills nullptr in the PDFProperties this with values from the PDFProperties extension, if possible
AcroFormReader::PDFProperties* AcroFormReader::PDFProperties::extend(  AcroFormReader::PDFProperties& extension ){
	this->ft = this->ft ? this->ft : extension.ft ? new std::string( *extension.ft ) : nullptr;
	this->ff = this->ff ? this->ff : extension.ff ? new int( *extension.ff ) : nullptr;
	this->da = this->da ? this->da : extension.da ? new std::string( *extension.da ) : nullptr;
	this->opt = this->opt ? this->opt : extension.opt ? new PDFArray( *extension.opt ) : nullptr;
	return this;
}

std::string AcroFormReader::toString( PDFObject *object ){
	if( object->GetType() == PDFObject::ePDFObjectLiteralString ){
		return ((PDFLiteralString*)object)->GetValue();
	} else if( object->GetType() == PDFObject::ePDFObjectHexString ){
		return ((PDFHexString*)object)->GetValue();
	} else if( object->GetType() == PDFObject::ePDFObjectName ){
		return ((PDFName*)object)->GetValue();
	} else{
		printf( "%i\n", object->GetType() );
		return nullptr;
	}
}

PDFDictionary *AcroFormReader::getAcroFormDict(){
	PDFDictionary *catalog = (PDFDictionary*)parser->QueryDictionaryObject( parser->GetTrailer(), "Root" );
	
	if( !catalog->Exists( "AcroForm" ))
		throw std::exception();

	PDFDictionary *acroform = (PDFDictionary*)parser->QueryDictionaryObject( catalog, "AcroForm" );
	delete catalog;
	return acroform;
}

AcroFormReader::AcroFormReader( char* file_path ){
	parser = new PDFParser();
	file = new InputFile();
	file->OpenFile( file_path );
	parser->StartPDFParsing( file->GetInputStream() );
	acro_form = getAcroFormDict();
}

AcroFormReader::~AcroFormReader(){
	for( auto& value : results )
		delete value;

	parser->ResetParser();
	delete parser;
	delete acro_form;
}

static void printPDFFieldValues( std::vector<AcroFormReader::PDFFieldValues*>* arr ){
	for( auto& a : *arr ){
		printf( "%s\n", a->name->c_str() );
		printPDFFieldValues( a->kids );
	}
}

void AcroFormReader::Parse( Character &character ){
	PDFArray *fieldarr = (PDFArray*)parser->QueryDictionaryObject( acro_form, "Fields" );

	std::vector<PDFFieldValues*>* arr = parseFieldArr( fieldarr, PDFProperties(), "" );

	printPDFFieldValues( arr );

	delete fieldarr;
}

std::vector<AcroFormReader::PDFFieldValues*>* AcroFormReader::parseFieldArr( PDFArray *array, PDFProperties inherited_props, std::string base_name ){

	std::vector<PDFFieldValues*>* result = new std::vector<PDFFieldValues*>();

	for( size_t i = 0; i < array->GetLength(); i++ ){
		PDFObject *obj = parser->QueryArrayObject( array, i );

		PDFFieldValues* value = parseField( (PDFDictionary*)obj, inherited_props, base_name );
		if( value )
			result->push_back( value );

		delete obj;
	}

	if( !result->size() ){
		delete result;
		return nullptr;
	}
	return result;
}

AcroFormReader::PDFFieldValues* AcroFormReader::parseField( PDFDictionary *dict, PDFProperties inherited_props, std::string base_name ){
	PDFObject* t = safeQuery( parser, dict, "T" );
	std::string fieldname = t ? toString( t ) : "";
	SAFE_DELETE( t );

	PDFObject* tu = safeQuery( parser, dict, "TU" );
	std::string fieldname_alt = t ? toString( tu ) : "";
	SAFE_DELETE( tu );

	PDFObject* tm = safeQuery( parser, dict, "TM" );
	std::string fieldname_map = t ? toString( tm ) : "";
	SAFE_DELETE( tm );

	PDFObject* ff = safeQuery( parser, dict, "Ff" );
	int flags = ff ? ((PDFInteger*)ff)->GetValue() : 0;
	SAFE_DELETE( ff );

	flags = flags ? flags : inherited_props.ff ? *inherited_props.ff : 0;

	if( fieldname == "" && 
			!dict->Exists( "Kids" ) &&
			dict->Exists( "Subtype" ) ){
		PDFObject* obj = parser->QueryDictionaryObject( dict, "Subtype" );
		if( toString( obj ) == "Widget" )
			return nullptr;
	}

	PDFFieldValues *result = new PDFFieldValues({
		new std::string(fieldname),															//Fieldname
		fieldname != "" ? new std::string( base_name + fieldname ) : new std::string( "" ),	//Complete Name
		new std::string( fieldname_alt ),													//Alternate Name
		new std::string( fieldname_map ),													//Mapping name
		nullptr,																			//Type
		nullptr,																			//Value
		new bool((flags >> 2) & 1 ),														//Export?
		nullptr,																			//Kids
	});
	
	results.push_back( result );

	PDFObject* kids = safeQuery( parser, dict, "Kids" );

	PDFProperties own_props = {
		new std::string( inherited_props.ft ? *inherited_props.ft : "" ),
		new int( flags ),
		//new std::string( inherited_props.da ? *inherited_props.da : "" ),
		new std::string( "" ),
		inherited_props.opt
	};

	if( kids ){
		std::vector<PDFFieldValues*>* kid_values = parseKids( dict, own_props, base_name + fieldname + "." );
		if( kid_values->size() != 0 ){
			result->kids = kid_values;
		}else {
			//Leaf
			parseFieldsValueData( result, dict, flags, &own_props );
		}
	}else {
		//Also Leaf
		parseFieldsValueData( result, dict, flags, &own_props );
	}

	return result;

}

std::vector<AcroFormReader::PDFFieldValues*>* AcroFormReader::parseKids( PDFDictionary* dict, AcroFormReader::PDFProperties inherited_props, std::string base_name ){
	
	PDFProperties local_props;
	
	PDFObject* ft = safeQuery( parser, dict, "FT" );
	local_props.ft = ft ? new std::string(	toString( ft )) : nullptr;
	SAFE_DELETE( ft );
	
	PDFObject* ff = safeQuery( parser, dict, "Ff" );
	local_props.ff = ff ? new int( ((PDFInteger*)ff)->GetValue() ) : nullptr;
	SAFE_DELETE( ff );

	PDFObject* da = safeQuery( parser, dict, "DA" );
	local_props.da = da ? new std::string( toString( da )) : nullptr;
	SAFE_DELETE( da );

	local_props.opt = (PDFArray*)safeQuery( parser, dict, "Opt" );

	local_props.extend( inherited_props );

	PDFObject* kid_arr = safeQuery( parser, dict, "Kids" );
	std::vector<PDFFieldValues*>* result = parseFieldArr( (PDFArray*)kid_arr, local_props, base_name );
	delete kid_arr;
	return result;
}


void AcroFormReader::parseFieldsValueData( AcroFormReader::PDFFieldValues* result, PDFDictionary* dict, int flags, PDFProperties* inherited_props ){
	PDFObject* ft = safeQuery( parser, dict, "FT" );
	std::string* field_type;
	if( ft ){
		field_type = new std::string(toString( ft ));
		delete ft;
	}else {
		field_type = inherited_props->ft;
	}

	if( !field_type )
		return; //k. Widgets apperently are a thing

	switch( stoEFieldType.find( *field_type )->second ){
		case eBtn:
			{
				if(( flags >> 16 ) & 1 ){
					result->type = new std::string( "button" );
				}
				else if(( flags >> 15 ) & 1 ){
					result->type = new std::string( "radio" );
					result->value = parseRadioButtonValue( dict );
				}
				else {
					result->type = new std::string( "checkbox" );
					result->value = parseOnOffValue( dict );
				}
				break;
			}
		case eTx:
			{
				if(( flags >> 25 ) & 1 ){
					result->type = new std::string( "richtext" );
					result->value = parseRichTextFieldValue( dict );
				}
				else {
					result->type = new std::string( "plaintext" );
					result->value = parseTextValue( dict );
				}
				break;
			}
		case eCh:
			{
				result->type = new std::string( "choice" );
				result->value = parseChoiceValue( dict );
				break;
			}
		case eSig:
			{
				result->type = new std::string( "signature" );
				break;
			}
	}
}

static std::string* parseTextField( PDFParser* parser, PDFDictionary* dict, std::string fieldName ){
	PDFObject* field = safeQuery( parser, dict, fieldName );
	std::string* ret = nullptr;
	if( !field )
		return nullptr;
	if( field->GetType() == PDFObject::ePDFObjectLiteralString ){
		ret = new std::string( ((PDFLiteralString*)field)->GetValue() );

	}else {
		printf( "TODO: FixMe: Unimplemented if-branch" );
		//TODO unimplemented PDFObjectStream
		throw new std::exception();
	}

	return ret;
}

PDFValue* AcroFormReader::parseChoiceValue( PDFDictionary* dict ){
	PDFObject* field = safeQuery( parser, dict, "V" );

	std::string* value = new std::string();

	if( field->GetType() == PDFDictionary::ePDFObjectArray ){
		printf( "TODO: FixMe: Unimplemented if-branch parseChoiceValue" );
		//TODO
		throw new std::exception();
	}
	else if( field->GetType() == PDFDictionary::ePDFObjectLiteralString ){
		*value += ((PDFLiteralString*)field)->GetValue();
	}
	else if( field->GetType() == PDFDictionary::ePDFObjectHexString ){
		*value += ((PDFHexString*)field)->GetValue();
	}

	PDFChoiceValue* ret = new PDFChoiceValue();

	ret->type = eChoiceValue;
	ret->text = value;

	return ret;
}

PDFValue* AcroFormReader::parseTextValue( PDFDictionary* dict ){
	PDFTextValue* ret = new PDFTextValue();
	ret->type = eTextValue;
	ret->text = parseTextField( parser, dict, "V" );

	return ret;
}

PDFValue* AcroFormReader::parseRichTextFieldValue( PDFDictionary* dict ){
	PDFRichTextValue* ret = new PDFRichTextValue();
	ret->type = eRichTextValue;
	ret->text = parseTextField( parser, dict, "V" );
	ret->richText = parseTextField( parser, dict, "RV" );

	return ret;
}

PDFValue* AcroFormReader::parseOnOffValue( PDFDictionary* dict ){
	PDFObject* v = safeQuery( parser, dict, "V" );
	if( !v )
		return nullptr;

	std::string value = toString( v );

	bool result = true;
	if( value == "Off" || value == "" )
		result = false;

	PDFOnOffValue* ret = new PDFOnOffValue();
	ret->type = ePDFValueType::eOnOffValueType;
	ret->value = result;
	
	delete v;
	return ret;
}

PDFValue* AcroFormReader::parseRadioButtonValue( PDFDictionary* dict ){
	PDFObject* v = safeQuery( parser, dict, "V" );
	if( !v )
		return nullptr;
	std::string value = toString( v );
	delete v;

	if( value == "Off" || value == "" )
		return nullptr;

	PDFArray* kids = (PDFArray*)safeQuery( parser, dict, "Kids" );
	if( kids ){
		for( size_t i = 0; i < kids->GetLength(); i++ ){
			PDFObject* widgetDict = parser->QueryArrayObject( kids, i );
			if( widgetDict ){
				PDFObject* apDict = safeQuery( parser, (PDFDictionary*)widgetDict, "AP" );
				if( apDict ){
					PDFObject* nAppear = safeQuery( parser, (PDFDictionary*)apDict, "N" );
					if( nAppear ){
						if( ((PDFDictionary*)nAppear)->Exists( value )){
							delete nAppear;
							delete apDict;
							delete widgetDict;
							delete kids;

							PDFRadioButtonValue* result = new PDFRadioButtonValue();
							result->type = eRadioButtonValue;
							result->value = i;
							return result;
						}
						delete nAppear;
					}
					delete apDict;
				}
				delete widgetDict;
			}
		}
		delete kids;
		PDFRadioButtonValue* result = new PDFRadioButtonValue();
		result->type = eRadioButtonValue;
		result->value = -1;
		return result;
	}
	return nullptr;
}
