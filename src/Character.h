#pragma once

#include <string>
#include <map>
#include "Stats.h"
#include "Class.h"
#include "Abilities.h"
#include "Inventory.h"
#include "Concepts.h"

struct Character{

	public:
		std::string char_name;
		DND_Stats stats;
		std::vector<DND_Class> classes;
		std::string background;
		std::string race;
		std::string alignment;
		uint32_t xp;

		DND_Lifemanagement lifemanagement;

		DND_Abilities abilities;

		DND_Inventory inventory;
		DND_Equipment equipment;

		uint8_t armor_class;
		bool using_shield;

		uint8_t darkvision;
		int8_t initiative;
		DND_Speed speed;

		std::vector<std::string> conditions;
		std::vector<std::string> languages;

		std::vector<std::string> proficiencies;
		std::vector<std::string> features_and_traits;
};
