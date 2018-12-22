#pragma once

#include "Character.h"
#include "AcroFormValue.h"

#include <PDFWriter/PDFArray.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFObjectCast.h>

#include <vector>
#include <stdio.h>
#include <memory>

class AcroFormReader{
	public:
		AcroFormReader() = default;
		~AcroFormReader() = default;

		int Parse( Character &character, const char* path );

		struct PDFFieldValues{
			std::unique_ptr<std::string> name;
			std::unique_ptr<std::string> full_name;
			std::unique_ptr<std::string> alt_name;
			std::unique_ptr<std::string> map_name;
			std::unique_ptr<std::string> type;
			std::unique_ptr<PDFValue> value;
			std::unique_ptr<bool> do_not_export;
			std::vector<std::unique_ptr<PDFFieldValues>> kids;
		};


	private:
		PDFParser parser;
		InputFile pdf;

		struct PDFProperties{
			std::string ft;
			int ff;
			std::string da;
			PDFObjectCastPtr<PDFArray> opt;

			void merge( std::string ft, int ff, std::string da, PDFObjectCastPtr<PDFArray> opt ){
				ft = ft != "" ? ft : this->ft;
				ff = ff ? ff : this->ff;
				da = da != "" ? da : this->da;
				opt = opt.GetPtr() ? opt : this->opt;
			}

		};
/*
		void parseFieldsValueData( PDFFieldValues* result, PDFDictionary* dict, int flags, PDFProperties* inherited_props );
*/
		int parseFieldArray( PDFArray* array, PDFProperties inherited_props, std::string base_name, std::vector<std::unique_ptr<PDFFieldValues>>& result );

		int parseKids( PDFDictionary* dict, PDFProperties inherited_props, std::string base_name, std::vector<std::unique_ptr<PDFFieldValues>>& result );

		int parseField( PDFDictionary* dict, PDFProperties inherited_props, std::string base_name, PDFFieldValues& result );
/*
		PDFValue* parseTextValue			( PDFDictionary* dict );
		PDFValue* parseRichTextFieldValue	( PDFDictionary* dict );
		PDFValue* parseOnOffValue			( PDFDictionary* dict );
		PDFValue* parseRadioButtonValue		( PDFDictionary* dict );
		PDFValue* parseChoiceValue			( PDFDictionary* dict );
*/
};
