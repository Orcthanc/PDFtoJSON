#pragma once

#include <map>
#include <memory>

class JSONObject {
	public:
		virtual ~JSONObject() = default;
		virtual void print( std::ofstream& file, uint8_t tabs );
		virtual void insert( std::string, std::string );
};

class JSONObjectDict: public JSONObject {
	public:
		virtual void insert( std::string key, std::string value );
		virtual ~JSONObjectDict();
		virtual void print( std::ofstream& file, uint8_t tabs );
	private:
		std::map<std::string, std::unique_ptr<JSONObject>> dict;
};

class JSONObjectValue: public JSONObject {
	public:
		JSONObjectValue( std::string value );
		virtual ~JSONObjectValue();
		virtual void print( std::ofstream& file, uint8_t tabs );

		std::string value;
};
