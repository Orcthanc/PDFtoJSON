#pragma once

#include "Character.h"
#include "AcroFormValue.h"

#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFArray.h>

#include <vector>
#include <stdio.h>

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
#define SAFE_DELETE( value ) if( value ) delete value;
				SAFE_DELETE( ft )
				SAFE_DELETE( ff )
				SAFE_DELETE( da )
				SAFE_DELETE( opt )
#undef SAFE_DELETE
			}
		};

		
		PDFParser* parser;
		InputFile* file;
		PDFDictionary *acro_form;
		std::vector<PDFFieldValues*> results;

		void parseFieldsValueData( PDFFieldValues* result, PDFDictionary* dict, int flags, PDFProperties* inherited_props );
		std::vector<PDFFieldValues*>* parseFieldArr( PDFArray *array, PDFProperties inherited_props, std::string base_name );
		std::vector<PDFFieldValues*>* parseKids( PDFDictionary* dict, PDFProperties inherited_props, std::string base_name );
		PDFFieldValues *parseField( PDFDictionary *dict, PDFProperties inherited_props, std::string base_name );
		PDFDictionary *getAcroFormDict();
		std::string toString( PDFObject * );

		PDFValue* parseTextValue			( PDFDictionary* dict );
		PDFValue* parseRichTextFieldValue	( PDFDictionary* dict );
		PDFValue* parseOnOffValue			( PDFDictionary* dict );
		PDFValue* parseRadioButtonValue		( PDFDictionary* dict );
		PDFValue* parseChoiceValue			( PDFDictionary* dict );

};
