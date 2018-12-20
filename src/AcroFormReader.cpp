#include "AcroFormReader.h"

#include <PDFWriter/PDFObjectCast.h>

#include <memory>

using namespace std;

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
		PDFObjectCastPtr<PDFDictionary> element( parser.QueryArrayObject( array, i ));
		PDFFieldValues* field_value = parseField( element.GetPtr(), inherited_props, base_name );

		if( field_value )
			result.push_back( unique_ptr<PDFFieldValues>( field_value ));
	}

	return 0;
}


AcroFormReader::PDFFieldValues* AcroFormReader::parseField( PDFDictionary* dict, PDFProperties inherited_props, string base_name ){


	return NULL;
}
