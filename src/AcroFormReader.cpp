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

	return 0;
}

int AcroFormReader::parseFieldArray( PDFArray* array, PDFProperties inherited_props, string base_name, vector<unique_ptr<PDFFieldValues>>& result ){

	for( size_t i = 0; i < array->GetLength(); ++i ){
		printf( "%lu\n", i );
		PDFObjectCastPtr<PDFDictionary> element( parser.QueryArrayObject( array, i ));
		PDFFieldValues* field_value = new PDFFieldValues();
		int failed_to_insert = parseField( element.GetPtr(), inherited_props, base_name, *field_value );

		if( !failed_to_insert )
			result.push_back( unique_ptr<PDFFieldValues>( field_value ));
	}

	return 0;
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
		return 1;
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
			//parseFieldsValueData();
		}
	}else {
		//parseFieldsValueData();
	}
	return 0;
}


int AcroFormReader::parseKids( PDFDictionary* dict, PDFProperties inherited_props, string base_name, vector<unique_ptr<PDFFieldValues>>& result ){
	return 0;
}
