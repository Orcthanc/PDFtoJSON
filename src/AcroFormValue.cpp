#include "AcroFormValue.h"

#include <sstream>

using namespace std;

bool PDFValue::checkValueNotEmpty(){
	return false;
}

const string PDFValue::getAsString(){
	return "";
}

bool PDFOnOffValue::checkValueNotEmpty(){
	return true;
}

const string PDFOnOffValue::getAsString(){
	return value ? "On" : "Off";
}

bool PDFRadioButtonValue::checkValueNotEmpty(){
	return true;
}

const string PDFRadioButtonValue::getAsString(){
	stringstream sstr;
	sstr << value;
	return sstr.str();
}

bool PDFTextValue::checkValueNotEmpty(){
	return text != "";
}

const string PDFTextValue::getAsString(){
	return text;
}

bool PDFRichTextValue::checkValueNotEmpty(){
	return text != "";
}

const string PDFRichTextValue::getAsString(){
	return text;
}

bool PDFChoiceValue::checkValueNotEmpty(){
	return true;
}

const string PDFChoiceValue::getAsString(){
	return text;
}
