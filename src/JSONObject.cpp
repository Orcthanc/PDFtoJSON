#include "JSONObject.h"

#include <fstream>
#include <iostream>

using namespace std;

void JSONObjectDict::print( std::ofstream& file, uint8_t tabs ){
	file << "{";

	for( auto it = dict.begin(); it != dict.end(); ++it ){
		if( it != dict.begin() )
			file << ',';
		file << '\n';
		file << std::string( tabs + 1, '\t' ) << "\"" << it->first << "\": ";
		it->second->print( file, tabs + 1 );
	}

	file << '\n' << std::string( tabs, '\t' ) << '}';
}

void JSONObjectDict::insert(std::string name, std::string text ){
	size_t pos;
	if(( pos = name.find( '.' )) != name.npos ){
		auto value = dict.find( name.substr( 0, pos ));
		if( value == dict.end() ){
			value = dict.insert({ name.substr( 0, pos ), unique_ptr<JSONObjectDict>(new JSONObjectDict() )}).first;
		}
		value->second->insert( name.substr( pos + 1 ), text );
	} else{
		dict.insert({ name, unique_ptr<JSONObjectValue>(new JSONObjectValue( text ))});
	}
}

void JSONObjectValue::print( std::ofstream& file, uint8_t tabs ){
	file << "\"" << value << "\"";
}

JSONObjectValue::JSONObjectValue( std::string name ){
	value = name;
}

JSONObjectDict::~JSONObjectDict(){}

JSONObjectValue::~JSONObjectValue(){}

void JSONObject::insert( std::string, std::string ){}

void JSONObject::print( std::ofstream&, uint8_t ){}
