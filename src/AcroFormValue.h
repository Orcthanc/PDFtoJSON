
enum ePDFValueType {
	eOnOffValueType,
	eRadioButtonValue,
	eRichTextValue,
	eTextValue,
	eChoiceValue,
};

struct PDFValue {
	ePDFValueType type;
};

struct PDFOnOffValue : PDFValue {
	
};

struct PDFRadioButtonValue : PDFValue {
	
};

struct PDFRichTextValue : PDFValue {

};

struct PDFTextValue : PDFValue {

};

struct PDFChoiceValue : PDFValue {

};
