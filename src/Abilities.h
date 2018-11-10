#pragma once

#include <string>

enum DND_AbilityType {
	#define ABILITYSTR( name ) name,
	#include "abilities.inl"
};

struct DND_Ability {
	std::string name;
	bool proficiency;
	bool mastery;

	int8_t value;
};

struct DND_Abilities {

	#define ABILITYSTR( name ) DND_Ability name = { #name, false, false, 0 };
	#define ABILITYDEX( name ) DND_Ability name = { #name, false, false, 0 };
	#define ABILITYCON( name ) DND_Ability name = { #name, false, false, 0 };
	#define ABILITYINT( name ) DND_Ability name = { #name, false, false, 0 };
	#define ABILITYWIS( name ) DND_Ability name = { #name, false, false, 0 };
	#define ABILITYCHA( name ) DND_Ability name = { #name, false, false, 0 };
	#include "abilities.inl"
};
