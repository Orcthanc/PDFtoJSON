#include "AcroFormReader.h"

#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFArray.h>

#include <string>

std::string AcroFormReader::toString( PDFObject &object ){
	if( object.GetType() == PDFObject::ePDFObjectLiteralString ){
		return ((PDFLiteralString*)&object)->GetValue();
	} else if( object.GetType() == PDFObject::ePDFObjectHexString ){
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
	parser.ResetParser();
	delete acro_form;
}

void AcroFormReader::Parse( Character &character ){
	PDFArray *fieldarr = (PDFArray*)parser.QueryDictionaryObject( acro_form, "Fields" );
	


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

bool AcroFormReader::parseField( PDFDictionary *dict, PDFProperties inherited_props, std::string base_name ){

}
