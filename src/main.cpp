#include <stdio.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFParser.h>

int main( int argc, char** argv ){
	if( argc < 2 ){
		printf( "Usage: PDFtoJSON <path_to_pdf.pdf>\n" );
		return 1;
	}

	InputFile pdf_file;
	PDFParser parser;

	pdf_file.OpenFile( argv[1] );
	parser.StartPDFParsing( pdf_file.GetInputStream() );
	
}
