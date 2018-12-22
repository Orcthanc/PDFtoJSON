#include <stdint.h>
#include <string>

enum ePDFValueType {
	eNoValue,
	eOnOffValue,
	eRadioButtonValue,
	eRichTextValue,
	eTextValue,
	eChoiceValue,
};

struct PDFValue {
	ePDFValueType type;
};

struct PDFOnOffValue : PDFValue {
	bool value;
};

struct PDFRadioButtonValue : PDFValue {
	uint8_t value;
};

struct PDFTextValue : PDFValue {
	std::string text;
};

struct PDFRichTextValue : PDFTextValue {
	std::string richText;
};

struct PDFChoiceValue : PDFValue {
	std::string text;
};
