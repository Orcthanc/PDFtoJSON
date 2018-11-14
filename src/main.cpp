#include "Character.h"
#include "AcroFormReader.h"

#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>

int main( int argc, char** argv ){
	if( argc < 2 ){
		printf( "Usage: PDFtoJSON <path_to_pdf.pdf>\n" );
		return 1;
	}

	AcroFormReader reader = AcroFormReader( argv[1] );
}
