#include "AcroFormReader.h"

#include <PDFWriter/PDFObjectCast.h>
#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFInteger.h>

#include <memory>

using namespace std;

static string toString( PDFObject* obj ){
	switch( obj->GetType() ){
		case PDFArray::ePDFObjectLiteralString:
			return ((PDFLiteralString*)obj)->GetValue();
		case PDFArray::ePDFObjectHexString:
			return ((PDFHexString*)obj)->GetValue();
		case PDFArray::ePDFObjectName:
			return ((PDFName*)obj)->GetValue();
		default:
			printf( "Found type %i while parsing pdf when excpected a string", obj->GetType() );
			return "";
	}
}

static string safeQueryToString( PDFParser& parser, PDFDictionary* dict, const char* query ){
	if( !dict->Exists( query ))
		return "";

	RefCountPtr<PDFObject> string_object( parser.QueryDictionaryObject( dict, query ));

	return toString( string_object.GetPtr() );
}

static int safeQueryToInteger( PDFParser& parser, PDFDictionary* dict, const char* query ){
	if( !dict->Exists( query ))
		return 0;

	PDFObjectCastPtr<PDFInteger> int_object( parser.QueryDictionaryObject( dict, query ));

	return int_object->GetValue();
}

static void printValue( PDFValue const* value ){
	if( !value ){
		printf( "\n" );
		return;
	}
	switch( value->type ){
		case eNoValue:
			printf( "\n" );
			break;
		case eOnOffValue:
			printf( "%s\n", ((PDFOnOffValue*)value)->value ? "On" : "Off" );
			break;
		case eRadioButtonValue:
			printf( "%u\n", ((PDFRadioButtonValue*)value)->value );
			break;
		case eRichTextValue:
			printf( "%s\n", ((PDFRichTextValue*)value)->text.c_str() );
			break;
		case eTextValue:
			printf( "%s\n", ((PDFTextValue*)value)->text.c_str() );
			break;
		case eChoiceValue:
			printf( "%s\n", ((PDFChoiceValue*)value)->text.c_str() );
			break;
	}
}

static void printParsedThings( vector<unique_ptr<AcroFormReader::PDFFieldValues>> const&  values ){
	for( auto& a : values ){
		if( a->full_name && *a->full_name != "" ){
			printf( "%s: %s:\t\t", a->full_name->c_str(), ""/*a->type->c_str()*/ );
			printValue( a->value.get() );
		}

		printParsedThings( a->kids );
	}
}

int AcroFormReader::Parse( Character &character, const char* path ){
	pdf.OpenFile( path );
	parser.StartPDFParsing( pdf.GetInputStream() );

	PDFObjectCastPtr<PDFDictionary> catalog( parser.QueryDictionaryObject( parser.GetTrailer(), "Root" ));

	if( !catalog->Exists( "AcroForm" ))
		return 1;

	PDFObjectCastPtr<PDFDictionary> acro_form( parser.QueryDictionaryObject( catalog.GetPtr(), "AcroForm" ));

	PDFObjectCastPtr<PDFArray> digital_form_fields( parser.QueryDictionaryObject( acro_form.GetPtr(), "Fields" ));

	vector<unique_ptr<PDFFieldValues>> result;
	PDFProperties inherited_props = { "", 0, "", NULL };

	parseFieldArray( digital_form_fields.GetPtr(), inherited_props, "", result );

	printf( "%lu\n", result.size() );

	printParsedThings( result );

	return 0;
}

int AcroFormReader::parseFieldArray( PDFArray* array, PDFProperties inherited_props, string base_name, vector<unique_ptr<PDFFieldValues>>& result ){

	for( size_t i = 0; i < array->GetLength(); ++i ){
		PDFObjectCastPtr<PDFDictionary> element( parser.QueryArrayObject( array, i ));
		PDFFieldValues* field_value = new PDFFieldValues();
		int parsed = parseField( element.GetPtr(), inherited_props, base_name, *field_value );

		if( parsed )
			result.push_back( unique_ptr<PDFFieldValues>( field_value ));
		else
			delete field_value;
	}

	return result.size();
}


int AcroFormReader::parseField( PDFDictionary* dict, PDFProperties inherited_props, string base_name, PDFFieldValues& result ){
	string localNameT = safeQueryToString( parser, dict, "T" );
	string localNameTU = safeQueryToString( parser, dict, "TU" );
	string localNameTM = safeQueryToString( parser, dict, "TM" );
	int local_flags = safeQueryToInteger( parser, dict, "Ff" );
	int flags = local_flags ? local_flags : inherited_props.ff;

	if( localNameT == "" && 
			!dict->Exists( "Kids" ) && 
			safeQueryToString( parser, dict, "Subtype" ) == "Widget" ){
		return 0;
	}

	//TODO check if corrupted variables
	
	result.name = unique_ptr<string>( new string( localNameT ));
	result.full_name = unique_ptr<string>( new string( localNameT != "" ? base_name + localNameT : "" ));
	result.alt_name = unique_ptr<string>( new string( localNameTU ));
	result.map_name = unique_ptr<string>( new string( localNameTM ));
	result.do_not_export = unique_ptr<bool>( new bool(( flags >> 2 ) & 1 ));

	if( dict->Exists( "Kids" )){
		vector<unique_ptr<PDFFieldValues>> kids;
		int kids_avaible = parseKids( dict, inherited_props, base_name + localNameT + ".", kids );
		if( kids_avaible ){
			//kids
			result.kids = move( kids );
		}else {
			goto parseFieldValueData;
		}
	}else {
		parseFieldValueData:

		std::unique_ptr<PDFFieldValues> value( new PDFFieldValues );
		parseFieldsValueData( dict, flags, inherited_props, value );
	}
	return 1;
}


int AcroFormReader::parseKids( PDFDictionary* dict, PDFProperties inherited_props, string base_name, vector<unique_ptr<PDFFieldValues>>& result ){

	std::string ft = safeQueryToString( parser, dict, "FT" );
	int ff = safeQueryToInteger( parser, dict, "Ff" );
	std::string da = safeQueryToString( parser, dict, "DA" );
	PDFObjectCastPtr<PDFArray> arr = parser.QueryDictionaryObject( dict, "Opt" );

	inherited_props.merge( ft, ff, da, arr );

	PDFObjectCastPtr<PDFArray> childs = parser.QueryDictionaryObject( dict, "Kids" );

	parseFieldArray( childs.GetPtr(), inherited_props, base_name, result );

	return 1;
}

int AcroFormReader::parseFieldsValueData( PDFDictionary* dict, int flags, PDFProperties const& inherited_props, unique_ptr<PDFFieldValues>& result ){
	string local_field_type = safeQueryToString( parser, dict, "FT" );
	string field_type = local_field_type == "" ? inherited_props.ft : local_field_type;

	if( field_type == "" )
		return 0;


	if( field_type == "Btn" ){
		if(( flags >> 16 ) & 1 ){
			//push-button
			result->type = unique_ptr<string>( new string( "button" ));
		}else if(( flags >> 15 ) & 1 ){
			//radio-button
			parseRadioButtonValue( dict, result );
		}else {
			//checkbox
			parseOnOffValue( dict, result );
		}
	}else if( field_type == "Tx" ){
		//Text
		if(( flags>>25 ) & 1) {
			parseRichTextFieldValue( dict, result );
		}else {
			parseTextValue( dict, result );
		}
	}else if( field_type == "Ch" ){
		parseChoiceValue( dict, result );
	}else if( field_type == "Sig" ){
		result->type = unique_ptr<string>( new string( "Signature" ));
	}else {
		printf( "Found unknown field-type %s.\n", field_type.c_str() );
	}

	return 1;
}

void AcroFormReader::parseTextValue( PDFDictionary* dict, std::unique_ptr<PDFFieldValues>& result ){
	result->type = unique_ptr<string>( new string( "plaintext" ));

	unique_ptr<PDFTextValue> value( new PDFTextValue() );
	value->type = eTextValue;
	value->text = safeQueryToString( parser, dict, "V" );

	result->value = move( value );
}

void AcroFormReader::parseRichTextFieldValue( PDFDictionary* dict, std::unique_ptr<PDFFieldValues>& result ){
	result->type = unique_ptr<string>( new string( "richtext" ));

	unique_ptr<PDFRichTextValue> value( new PDFRichTextValue() );
	value->type = eTextValue;
	value->text = safeQueryToString( parser, dict, "V" );
	value->richText = safeQueryToString( parser, dict, "RV" );

	result->value = move( value );
}

void AcroFormReader::parseOnOffValue( PDFDictionary* dict, std::unique_ptr<PDFFieldValues>& result ){
	result->type = unique_ptr<string>( new string( "checkbox" ));

	unique_ptr<PDFOnOffValue> value( new PDFOnOffValue() );

	value->type = eOnOffValue;

	string is_on = safeQueryToString( parser, dict, "V" );
	if( is_on == "" || is_on == "Off" ){
		value->value = false;
	}else {
		value->value = true;
	}

	result->value = move( value );
}

void AcroFormReader::parseRadioButtonValue( PDFDictionary* dict, std::unique_ptr<PDFFieldValues>& result ){
	result->type = unique_ptr<string>( new string( "radio" ));

	if( !dict->Exists( "V" )){
		//No button at all
		result->value = unique_ptr<PDFValue>( new PDFValue{ eNoValue } );
	}else {
		string button_value = safeQueryToString( parser, dict, "V" );
		if( button_value == "" || button_value == "Off" ){
			//Botched checkbox thats unchecked
			unique_ptr<PDFOnOffValue> value( new PDFOnOffValue() );
			value->type = eOnOffValue;
			value->value = false;
			result->value = move( value );
		}else {
			if( !dict->Exists( "Kids" )){
				//Botched but checked checkbox
				unique_ptr<PDFOnOffValue> value( new PDFOnOffValue() );
				value->type = eOnOffValue;
				value->value = true;
				result->value = move( value );
			}else {
				PDFObjectCastPtr<PDFArray> kids( parser.QueryDictionaryObject( dict, "Kids" ));
				for( size_t i = 0; i < kids->GetLength(); i++ ){
					PDFObjectCastPtr<PDFDictionary> widgetDict( parser.QueryArrayObject( kids.GetPtr(), i ));
					PDFObjectCastPtr<PDFDictionary> apDict( parser.QueryDictionaryObject( widgetDict.GetPtr(), "AP" ));
					PDFObjectCastPtr<PDFDictionary> apNameDict( parser.QueryDictionaryObject( apDict.GetPtr(), "N" ));
					if( apNameDict->Exists( button_value )){
						unique_ptr<PDFRadioButtonValue> value( new PDFRadioButtonValue );
						value->type = eRadioButtonValue;
						value->value = i;
						result->value = move( value );
						break;
					}
				}
			}
		}
	}
	result->value = unique_ptr<PDFValue>( new PDFValue{ eNoValue } );
}

void AcroFormReader::parseChoiceValue( PDFDictionary* dict, std::unique_ptr<PDFFieldValues>& result ){
	result->type = unique_ptr<string>( new string( "choice" ));

	unique_ptr<PDFChoiceValue> value( new PDFChoiceValue() );
	value->type = eTextValue;
	value->text = safeQueryToString( parser, dict, "V" );

	result->value = move( value );
}

