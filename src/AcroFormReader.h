#pragma once

#include "Character.h"
#include "AcroFormValue.h"

#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>

#include <vector>
#include <stdio.h>
#include <memory>

class AcroFormReader{
	public:
		AcroFormReader() = default;
		~AcroFormReader() = default;

		int Parse( Character &character, const char* path );

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
		PDFParser parser;
		InputFile pdf;

		struct PDFProperties{
			std::string ft;
			int ff;
			std::string da;
			PDFArray *opt;

			void merge( std::string ft, int ff, std::string da, PDFArray* opt ){
				ft = ft != "" ? ft : this->ft;
				ff = ff ? ff : this->ff;
				da = da != "" ? da : this->da;
				opt = opt ? opt : this->opt;
			}

			~PDFProperties(){
				if( opt )
					delete opt;
				opt = NULL;
			}
		};
/*
		void parseFieldsValueData( PDFFieldValues* result, PDFDictionary* dict, int flags, PDFProperties* inherited_props );
*/
	int parseFieldArray( PDFArray* array, PDFProperties inherited_props, std::string base_name, std::vector<std::unique_ptr<PDFFieldValues>>& result );
/*
		std::vector<PDFFieldValues*>* parseKids( PDFDictionary* dict, PDFProperties inherited_props, std::string base_name );
*/
		PDFFieldValues* parseField( PDFDictionary* dict, PDFProperties inherited_props, std::string base_name );
/*
		PDFDictionary *getAcroFormDict();
		std::string toString( PDFObject * );

		PDFValue* parseTextValue			( PDFDictionary* dict );
		PDFValue* parseRichTextFieldValue	( PDFDictionary* dict );
		PDFValue* parseOnOffValue			( PDFDictionary* dict );
		PDFValue* parseRadioButtonValue		( PDFDictionary* dict );
		PDFValue* parseChoiceValue			( PDFDictionary* dict );
*/
};
