#pragma once

#include "Character.h"

#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFArray.h>

#include <vector>

class AcroFormReader{
	public:
		AcroFormReader( char *input_file );
		~AcroFormReader();

		void Parse( Character &character );

		struct PDFFieldValues{
			std::string name;
			std::string full_name;
			std::string alt_name;
			std::string map_name;
			bool do_not_export;
		};


	private:
		struct PDFProperties{
			std::string ft;
			int ff;
			std::string da;
			PDFArray opt;
		};

		
		PDFParser parser;
		PDFDictionary *acro_form;
		std::vector<PDFFieldValues*> results;

		bool parseFieldArr( PDFArray *array, PDFProperties inherited_props, std::string base_name );
		PDFFieldValues *parseField( PDFDictionary *dict, PDFProperties inherited_props, std::string base_name );
		PDFDictionary *getAcroFormDict();
		std::string toString( PDFObject * );
		
};
