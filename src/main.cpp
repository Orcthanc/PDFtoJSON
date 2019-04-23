#include "JSONObject.h"
#include "AcroFormReader.h"

#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>

#include <chrono>
#include <iostream>
#include <fstream>

using namespace std;
using namespace std::chrono;

int main( int argc, char** argv ){
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	if( argc < 3 ){
		printf( "Usage: PDFtoJSON <path_to_pdf.pdf> <path_to_output.json>\n" );
		return 1;
	}

	AcroFormReader reader = AcroFormReader();

	JSONObject* obj;

	reader.Parse( &obj, argv[1] );

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	
	cout <<duration_cast<microseconds>(t2 - t1).count() << "us execution-time" << endl;

	std::ofstream file;
	file.open( argv[2], ios::out | ios::trunc );

	obj->print( file, 0 );

	file << flush;
	file.close();

	t1 = high_resolution_clock::now();
	
	cout <<duration_cast<microseconds>(t1 - t2).count() << "us execution-time" << endl;
}
