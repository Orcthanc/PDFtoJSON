#pragma once

#include <string>
#include <vector>

struct DND_Description {
	uint16_t age;
	double height;
	double weight;
	std::string eyes;
	std::string skin;
	std::string hair;

	std::vector<std::string> allies_and_organizations;
	
	std::string traits;
	std::string ideals;
	std::string bonds;
	std::string flaws;

	std::string backstory;
};
