#include "AcroFormReader.h"

#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFInteger.h>

#include <string>

static PDFObject* safeQuery( PDFParser& parser, PDFDictionary *dict, std::string name ){
	return dict->Exists( name ) ? parser.QueryDictionaryObject( dict, name ) : nullptr;
}

std::string AcroFormReader::toString( PDFObject *object ){
	if( object->GetType() == PDFObject::ePDFObjectLiteralString ){
		return ((PDFLiteralString*)&object)->GetValue();
	} else if( object->GetType() == PDFObject::ePDFObjectHexString ){
		return ((PDFHexString*)&object)->GetValue();
	} else{
		return NULL;
	}
}

PDFDictionary *AcroFormReader::getAcroFormDict(){
	PDFDictionary *catalog = (PDFDictionary*)parser.QueryDictionaryObject( (PDFDictionary*)parser.GetTrailer(), "Root" );
	
	if( !catalog->Exists( "AcroForm" ))
		throw std::exception();

	PDFDictionary *acroform = (PDFDictionary*)parser.QueryDictionaryObject( catalog, "AcroForm" );
	delete catalog;
	return acroform;
}

AcroFormReader::AcroFormReader( char* file_path ){
	InputFile file;
	file.OpenFile( file_path );
	parser.StartPDFParsing( file.GetInputStream() );
	acro_form = getAcroFormDict();
}

AcroFormReader::~AcroFormReader(){
	for( auto& value : results )
		delete value;

	parser.ResetParser();
	delete acro_form;
}

void AcroFormReader::Parse( Character &character ){
	PDFArray *fieldarr = (PDFArray*)parser.QueryDictionaryObject( acro_form, "Fields" );
	
	parseFieldArr( fieldarr, PDFProperties(), "" );

	delete fieldarr;
}

bool AcroFormReader::parseFieldArr( PDFArray *array, PDFProperties inherited_props, std::string base_name ){

	bool not_empty = false;
	for( size_t i = 0; i < array->GetLength(); i++ ){
		PDFObject *obj = parser.QueryArrayObject( array, i );

		not_empty = not_empty ||
			parseField( (PDFDictionary*)obj, inherited_props, base_name );

		delete obj;
	}
	return not_empty;
}

AcroFormReader::PDFFieldValues* AcroFormReader::parseField( PDFDictionary *dict, PDFProperties inherited_props, std::string base_name ){
	PDFObject* t = safeQuery( parser, dict, "T" );
	std::string fieldname = t ? toString( t ) : "";
	delete t;

	PDFObject* tu = safeQuery( parser, dict, "TU" );
	std::string fieldname_alt = t ? toString( tu ) : "";
	delete tu;

	PDFObject* tm = safeQuery( parser, dict, "TM" );
	std::string fieldname_map = t ? toString( tm ) : "";
	delete tm;

	PDFObject* ff = safeQuery( parser, dict, "Ff" );
	int flags = ff ? ((PDFInteger*)ff)->GetValue() : 0;
	delete ff;

	flags = flags ? flags : inherited_props.ff;

	if( fieldname == "" && 
			!dict->Exists( "Kids" ) &&
			dict->Exists( "Subtype" ) ){
		PDFObject* obj = parser.QueryDictionaryObject( dict, "Subtype" );
		if( toString( obj ) == "Widget" )
			return nullptr;
	}

	PDFFieldValues *result = new PDFFieldValues({
		fieldname,
		fieldname != "" ? base_name + fieldname : "",
		fieldname_alt,
		fieldname_map,
		((flags >> 2) & 1) == 1
	});
	
	results.push_back( result );

	PDFObject* kids = safeQuery( parser, dict, "Kids" );
	if( kids ){
		std::vector<PDFFieldValues*> kid_values = parseKids();
		if( kid_values.size() != 0 ){
			for( auto& kid_value : kid_values ){
				results.push_back( kid_value );
			}
		}else {
			//Leaf
			parseFieldsValueData();
		}
	}else {
		//Read Data
		parseFieldsValueData();
	}

}

