#include "Dictionary.h"

#include <iostream>
#include <fstream>

using namespace std;

int Dictionary::insert( string key, string value ){
	dictionary.insert({ key, value });
	return 1;
}

int Dictionary::initFromFile( std::string path ){
	ifstream file( path );
	string line;
	if( !file.is_open() )
		return 0;

	while( getline( file, line )){
		size_t delpos = line.find( "=" );
		dictionary.insert({ line.substr( 0, delpos ), line.substr( delpos + 1 )});
	}

	return 1;
}

string Dictionary::lookup( const string& value ){
	return dictionary.find( value )->second;
}
