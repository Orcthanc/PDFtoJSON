#include <stdio.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/MapIterator.h>
#include <PDFWriter/PDFName.h>

#include <map>

int main( int argc, char** argv ){
	if( argc < 2 ){
		printf( "Usage: PDFtoJSON <path_to_pdf.pdf>\n" );
		return 1;
	}

	InputFile pdf_file;
	PDFParser parser;

	pdf_file.OpenFile( argv[1] );
	parser.StartPDFParsing( pdf_file.GetInputStream() );

	printf( "%f %lu\n", parser.GetPDFLevel(), parser.GetPagesCount() );

	PDFDictionary *root = parser.GetTrailer();

	MapIterator<PDFNameToPDFObjectMap> it = root->GetIterator();

	while( it.MoveNext() ){
		printf( "%s\n", it.GetKey()->GetValue().c_str() );
	}
	
}
