#include "AcroFormValue.h"

bool PDFValue::checkValueNotEmpty(){
	return false;
}

bool PDFOnOffValue::checkValueNotEmpty(){
	return true;
}

bool PDFRadioButtonValue::checkValueNotEmpty(){
	return true;
}

bool PDFTextValue::checkValueNotEmpty(){
	return text != "";
}

bool PDFRichTextValue::checkValueNotEmpty(){
	return text != "";
}

bool PDFChoiceValue::checkValueNotEmpty(){
	return true;
}
