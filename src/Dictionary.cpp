#include "Dictionary.h"

#include <iostream>
#include <fstream>

using namespace std;

int Dictionary::insert( string key, string value ){
	dictionary.insert({ key, value });
	return 1;
}

int Dictionary::initFromFile( std::string path, bool parse ){
	ifstream file( path );
	string line;
	if( !file.is_open() )
		return 0;

	while( getline( file, line )){
		if( line.c_str()[0] == '#' )
			continue;
		size_t delpos = line.find( "=" );
		if( parse )
			dictionary.insert({ line.substr( delpos + 1 ), line.substr( 0, delpos )});
		else
			dictionary.insert({ line.substr( 0, delpos ), line.substr( delpos + 1 )});
	}

	return 1;
}

bool Dictionary::exists( const string& value ) const{
	return dictionary.find( value ) != dictionary.end();
}

string Dictionary::lookup( const string& value ) const{
	auto result = dictionary.find( value );
	return result != dictionary.end() ? result->second : "";
}
