#pragma once

#include "Character.h"
#include "AcroFormValue.h"

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
			std::string* name;
			std::string* full_name;
			std::string* alt_name;
			std::string* map_name;
			std::string* type;
			PDFValue* value;
			bool* do_not_export;
			std::vector<PDFFieldValues*>* kids;
		};


	private:
		struct PDFProperties{
			std::string* ft;
			int* ff;
			std::string* da;
			PDFArray* opt;
			
			PDFProperties* extend( PDFProperties& extension );
			~PDFProperties(){
#define SAFEDELETE( value ) if( value ) delete value;
				SAFEDELETE( ft )
				SAFEDELETE( ff )
				SAFEDELETE( da )
				SAFEDELETE( opt )
#undef SAFEDELETE
			}
		};

		
		PDFParser parser;
		PDFDictionary *acro_form;
		std::vector<PDFFieldValues*> results;

		void parseFieldsValueData( PDFFieldValues* result, PDFDictionary* dict, int flags, PDFProperties* inherited_props );
		std::vector<PDFFieldValues*>* parseFieldArr( PDFArray *array, PDFProperties inherited_props, std::string base_name );
		std::vector<PDFFieldValues*>* parseKids( PDFDictionary* dict, PDFProperties inherited_props, std::string base_name );
		PDFFieldValues *parseField( PDFDictionary *dict, PDFProperties inherited_props, std::string base_name );
		PDFDictionary *getAcroFormDict();
		std::string toString( PDFObject * );

};
