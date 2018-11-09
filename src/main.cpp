#include <stdio.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/MapIterator.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFIndirectObjectReference.h>

#include <map>
#include <exception>
#include <string>
#include <string.h>

const char* PDFObjectTypeToCString( PDFParser&, PDFObject&, char * );

void parseObject( PDFParser& parser, PDFObject& obj, int ident ){
	switch( obj.GetType() ){
		case PDFObject::ePDFObjectBoolean:
			break;
		case PDFObject::ePDFObjectLiteralString:
			break;
		case PDFObject::ePDFObjectHexString:
			break;
		case PDFObject::ePDFObjectNull:
			break;
		case PDFObject::ePDFObjectName:
			break;
		case PDFObject::ePDFObjectInteger:
			break;
		case PDFObject::ePDFObjectReal:
			break;
		case PDFObject::ePDFObjectArray:
			break;
		case PDFObject::ePDFObjectDictionary:
			{
				MapIterator<PDFNameToPDFObjectMap> it = ((PDFDictionary*)&obj)->GetIterator();

				char * pnt = nullptr;

				while( it.MoveNext() ){
					int length = snprintf( NULL, 0, "%s%s:\n", std::string( ident * 2, ' ' ).c_str(), it.GetKey()->GetValue().c_str() );
					printf( "%s%s:%s%s\n", 
							std::string( ident * 2, ' ' ).c_str(), 
							it.GetKey()->GetValue().c_str(), 
							std::string( 64 - length, ' ' ).c_str(),
							PDFObjectTypeToCString( parser, *it.GetValue(), pnt ) );
					if( pnt != nullptr )
						delete[] pnt;
					parseObject( parser, *it.GetValue(), ident + 1 );
				}
				break;
			}
		case PDFObject::ePDFObjectIndirectObjectReference:
			{
				parseObject( parser, *parser.ParseNewObject( ((PDFIndirectObjectReference*)&obj)->mObjectID ), ident );
			}
			break;
		case PDFObject::ePDFObjectStream:
			break;
		case PDFObject::ePDFObjectSymbol:
			break;
		default:
			throw new std::exception();
	}
}

const char* PDFObjectTypeToCString( PDFParser& parser, PDFObject& obj, char * ret ){
	switch( obj.GetType() ){
		case PDFObject::ePDFObjectBoolean:
			return "ePDFObjectBoolean";
		case PDFObject::ePDFObjectLiteralString:
			return "ePDFObjectLiteralString";
		case PDFObject::ePDFObjectHexString:
			return "ePDFObjectHexString";
		case PDFObject::ePDFObjectNull:
			return "ePDFObjectNull";
		case PDFObject::ePDFObjectName:
			return "ePDFObjectName";
		case PDFObject::ePDFObjectInteger:
			return "ePDFObjectInteger";
		case PDFObject::ePDFObjectReal:
			return "ePDFObjectReal";
		case PDFObject::ePDFObjectArray:
			return "ePDFObjectArray";
		case PDFObject::ePDFObjectDictionary:
			return "ePDFObjectDictionary";
		case PDFObject::ePDFObjectIndirectObjectReference:
			{
				char arr[33];

				strcpy( arr, PDFObjectTypeToCString( parser, *parser.ParseNewObject( ((PDFIndirectObjectReference*)&obj)->mObjectID ), nullptr));

				int len = snprintf( NULL, 0, "%s%s", arr,"ePDFObjectIndirectObjectReference" );

				char * arr2 = new char[70];

				snprintf( arr2, 69, "%s%s(%s)", arr, std::string( 69 - len, ' ' ).c_str(), "ePDFObjectIndirectObjectReference" );

				return arr2;
			}
		case PDFObject::ePDFObjectStream:
			return "ePDFObjectStream";
		case PDFObject::ePDFObjectSymbol:
			return "ePDFObjectSymbol";
		default:
			throw new std::exception();
	}
}

int main( int argc, char** argv ){
	if( argc < 2 ){
		printf( "Usage: PDFtoJSON <path_to_pdf.pdf>\n" );
		return 1;
	}

	InputFile pdf_file;
	PDFParser parser;

	pdf_file.OpenFile( argv[1] );
	parser.StartPDFParsing( pdf_file.GetInputStream() );

	printf( "Found PDF of Version: %f with %lu pages.\nBeginning parsing\n\n", parser.GetPDFLevel(), parser.GetPagesCount() );

	PDFDictionary *root = parser.GetTrailer();

	parseObject( parser, *root, 0 );
}
