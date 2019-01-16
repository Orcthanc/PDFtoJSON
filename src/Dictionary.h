#include <map>

class Dictionary{
	public:
		int insert( std::string key, std::string value );
		//If parse is true, init the map <Values in PDF, Reasonable Values>, else the other way around
		int initFromFile( std::string path, bool parse );
		std::string lookup( const std::string& value );

	private:
		std::map<std::string, std::string> dictionary;
};
