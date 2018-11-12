#include "Reader.h"
#include "Class.h"

#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFIndirectObjectReference.h>

#include <map>
#include <algorithm>

enum EPDFFormObjectTValuesType{
	#define VALUE( name ) e##name,
	#include "pdfFormObjectTValues.inl"
};

std::map<std::string, EPDFFormObjectTValuesType> stoT = {
#define VALUE( name ) { #name, e##name },
#include "pdfFormObjectTValues.inl"
};

static std::string sanitize( std::string string ){
	std::replace( string.begin(), string.end(), ' ', '_' );
	return string;
}

bool readObject( Character& character, PDFDictionary& object, PDFParser& parser, std::string prefix ){
	if( !object.Exists( "T" )){
		if( object.Exists( "V" )){
			printf( "Found weird object:\t\t\t%s\n", ((PDFLiteralString*)object.QueryDirectObject( "V" ))->GetValue().c_str() );
		}
		return false;
	}


	auto s_key = sanitize( ((PDFLiteralString*)object.QueryDirectObject( "T" ))->GetValue() );

	if( object.Exists( "Kids" )){
		return readArr( character, *(PDFArray*)object.QueryDirectObject( "Kids" ), parser, prefix + s_key + "__" );
		
	}
	
	if( !object.Exists( "V" ) ){
//		printf( "Dafaq? Probably found an incorrect form.\n" );
		return false;
	}

	std::string value = std::string(((PDFLiteralString*)object.QueryDirectObject( "V" ))->GetValue());

	auto key = stoT.find( s_key );
	if( key == stoT.end() ){
		printf( "Keyword %s%s not found:\t\t%s\n", prefix.c_str(), s_key.c_str(), sanitize( value ).c_str() );
		return false;
	}
	switch( key->second ){
	case eclass:
		if( character.classes.size() == 0 ){
			printf( "Created class\n" );
			character.classes.push_back( new DND_Class() );
		}
		character.classes[0]->name = value;
		return true;
	default:
		return false;
	}

	return false;
}

bool readArr( Character& character, PDFArray& arr, PDFParser& parser, std::string prefix ){
	bool ret = true;
	for( size_t i = 0; i < arr.GetLength(); i++ ){
		PDFObject* obj = arr.QueryObject( i );		//PDFIndirectObjectReference | PDFDictionary
		if( obj->GetType() == PDFObject::ePDFObjectIndirectObjectReference ){
			obj = parser.ParseNewObject( ((PDFIndirectObjectReference*)obj)->mObjectID );
		}
		if( !readObject( character, *(PDFDictionary*)obj, parser, prefix ) )
			ret = false;
	}
	return ret;
}


