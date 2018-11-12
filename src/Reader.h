#pragma once

#include "Character.h"

#include "PDFWriter/PDFParser.h"
#include "PDFWriter/PDFDictionary.h"

bool readObject( Character&, PDFDictionary& object, PDFParser&, std::string prefix );
bool readArr( Character&, PDFArray& object, PDFParser&, std::string prefix );
