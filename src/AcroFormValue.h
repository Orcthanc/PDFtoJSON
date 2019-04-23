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
	PDFValue() = default;
	PDFValue( ePDFValueType type ){
		this->type = type;
	}
	virtual ~PDFValue() = default;

	virtual bool checkValueNotEmpty();
	virtual const std::string getAsString();
};

struct PDFOnOffValue : public PDFValue {
	bool value;

	virtual bool checkValueNotEmpty();
	virtual const std::string getAsString();
};

struct PDFRadioButtonValue : public PDFValue {
	uint8_t value;

	virtual bool checkValueNotEmpty();
	virtual const std::string getAsString();
};

struct PDFTextValue : public PDFValue {
	std::string text;

	virtual bool checkValueNotEmpty();
	virtual const std::string getAsString();
};

struct PDFRichTextValue : public PDFTextValue {
	std::string richText;

	virtual bool checkValueNotEmpty();
	virtual const std::string getAsString();
};

struct PDFChoiceValue : public PDFValue {
	std::string text;

	virtual bool checkValueNotEmpty();
	virtual const std::string getAsString();
};
