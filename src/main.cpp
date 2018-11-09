#include <stdio.h>
#include <PDFWriter/InputFile.h>
#include <PDFWriter/PDFParser.h>
#include <PDFWriter/PDFDictionary.h>
#include <PDFWriter/MapIterator.h>
#include <PDFWriter/PDFName.h>
#include <PDFWriter/PDFObject.h>
#include <PDFWriter/PDFIndirectObjectReference.h>
#include <PDFWriter/PDFLiteralString.h>
#include <PDFWriter/PDFBoolean.h>
#include <PDFWriter/PDFInteger.h>
#include <PDFWriter/PDFHexString.h>
#include <PDFWriter/PDFReal.h>
#include <PDFWriter/PDFArray.h>

#include <map>
#include <exception>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>

#define MAX( a, b ) (a) > (b) ? (a) : (b)

const char* PDFObjectTypeToCString( PDFParser&, PDFObject&, char * );

std::vector<std::string> blacklist = {
	"Widths"
};

std::vector<ObjectIDType> objs;

void parseObject( PDFParser& parser, PDFObject& obj, int ident ){
	if( ident > 60 )
		return;
	switch( obj.GetType() ){
		case PDFObject::ePDFObjectBoolean:
			printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), ((PDFBoolean*)&obj)->GetValue() ? "true" : "false" );
			break;
		case PDFObject::ePDFObjectLiteralString:
			printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), ((PDFLiteralString*)&obj)->GetValue().c_str() );
			break;
		case PDFObject::ePDFObjectHexString:
			printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), ((PDFHexString*)&obj)->GetValue().c_str() );
			break;
		case PDFObject::ePDFObjectNull:
			printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), "NULL" );
			break;
		case PDFObject::ePDFObjectName:
			printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), ((PDFName*)&obj)->GetValue().c_str() );
			break;
		case PDFObject::ePDFObjectInteger:
			printf( "%s%lli\n", std::string( ident * 2, ' ' ).c_str(), ((PDFInteger*)&obj)->GetValue() );
			break;
		case PDFObject::ePDFObjectReal:
			printf( "%s%f\n", std::string( ident * 2, ' ' ).c_str(), ((PDFReal*)&obj)->GetValue() );
			break;
		case PDFObject::ePDFObjectArray:
			{
				for( size_t i = 0; i < ((PDFArray*)&obj)->GetLength(); i++ ){

					PDFObject * obj2 = ((PDFArray*)&obj)->QueryObject( i );

					if( obj2->GetType() == PDFObject::ePDFObjectIndirectObjectReference ){
						if( std::find( objs.begin(), objs.end(), ((PDFIndirectObjectReference*)obj2)->mObjectID ) != objs.end() )
							continue;
					}

					char * pnt = nullptr;					
					printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), PDFObjectTypeToCString( parser, *obj2, pnt ));
					parseObject( parser, *obj2, ident + 1 );
					if( pnt == nullptr )
						delete[] pnt;
				}
			}
			break;
		case PDFObject::ePDFObjectDictionary:
			{
				MapIterator<PDFNameToPDFObjectMap> it = ((PDFDictionary*)&obj)->GetIterator();

				char * pnt = nullptr;

				while( it.MoveNext() ){

					if( it.GetValue()->GetType() == PDFObject::ePDFObjectIndirectObjectReference ){
						if( std::find( objs.begin(), objs.end(), ((PDFIndirectObjectReference*)it.GetValue())->mObjectID ) != objs.end() )
							continue;
					}

					int length = snprintf( NULL, 0, "%s%s:\n", std::string( ident * 2, ' ' ).c_str(), it.GetKey()->GetValue().c_str() );
					printf( "%s%s:%s%s\n", 
							std::string( ident * 2, ' ' ).c_str(), 
							it.GetKey()->GetValue().c_str(), 
							std::string( MAX( 150 - length, 10 ), ' ' ).c_str(),
							PDFObjectTypeToCString( parser, *it.GetValue(), pnt ) );
					if( pnt != nullptr ){
						delete[] pnt;
						pnt = nullptr;
					}
					if( std::find( blacklist.begin(), blacklist.end(), it.GetKey()->GetValue() ) == blacklist.end() )
						parseObject( parser, *it.GetValue(), ident + 1 );
				}
				break;
			}
		case PDFObject::ePDFObjectIndirectObjectReference:
			{
				if( (std::find( objs.begin(), objs.end(), ((PDFIndirectObjectReference*)&obj)->mObjectID ) == objs.end() )){
					objs.push_back( ((PDFIndirectObjectReference*)&obj)->mObjectID );
					parseObject( parser, *parser.ParseNewObject( ((PDFIndirectObjectReference*)&obj)->mObjectID ), ident );
				}
			}
			break;
		//TODO
		case PDFObject::ePDFObjectStream:
			break;
		case PDFObject::ePDFObjectSymbol:
			printf( "%s%s\n", std::string( ident * 2, ' ' ).c_str(), "ERROR" );
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

				int len = snprintf( NULL, 0, "%s%s", arr,"(ePDFObjectIndirectObjectReference)" );

				char * arr2 = new char[70];

				snprintf( arr2, 70, "%s%s(%s)", arr, std::string( 69 - len, ' ' ).c_str(), "ePDFObjectIndirectObjectReference" );

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
