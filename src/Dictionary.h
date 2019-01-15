#include <map>

class Dictionary{
	public:
		int insert( std::string key, std::string value );
		int initFromFile( std::string path );
		std::string lookup( const std::string& value );

	private:
		std::map<std::string, std::string> dictionary;
};
