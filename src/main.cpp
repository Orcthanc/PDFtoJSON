#include "Character.h"
#include "AcroFormReader.h"

#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>

#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

int main( int argc, char** argv ){
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	if( argc < 2 ){
		printf( "Usage: PDFtoJSON <path_to_pdf.pdf>\n" );
		return 1;
	}

	AcroFormReader reader = AcroFormReader();

	Character character;

	reader.Parse( character, argv[1] );

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	
	cout <<duration_cast<microseconds>(t2 - t1).count() << "us execution-time" << endl;
}
