#pragma once

#include "Character.h"

#include "PDFWriter/PDFParser.h"
#include "PDFWriter/PDFDictionary.h"

enum EPDFFormObjectTValuesType{
	#define VALUE( name ) e##name,
	#include "pdfFormObjectTValues.inl"
};

extern std::map<std::string, EPDFFormObjectTValuesType> stoType;

bool readObject( Character&, PDFDictionary& object, PDFParser&, std::string prefix );
bool readArr( Character&, PDFArray& object, PDFParser&, std::string prefix );
