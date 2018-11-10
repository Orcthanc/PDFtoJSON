#pragma once

#include "Stats.h"
#include "Abilities.h"

#include <string>

enum DND_ItemeffectType {
	Weapon,
	Damage,
	Armor,
	Statbonus,
	Abilitybonus,
	Complex
};

struct DND_Itemeffect {
	DND_ItemeffectType sType;
	DND_Itemeffect* pNext;
};

struct DND_ItemeffectWeapon : DND_Itemeffect {
	int8_t atkbonus;
	int8_t dice_amount;
	int8_t dice_type;
	int8_t damage_bonus;
};

struct DND_ItemeffectDamage : DND_Itemeffect {
	int8_t dice_amount;
	int8_t dice_type;
	int8_t damage_bonus;
};

struct DND_ItemeffectArmor : DND_Itemeffect {
	int8_t armor_class_modifier;
};

struct DND_ItemeffectStatboost : DND_Itemeffect {
	DND_stat_names name_of_stat;
	int8_t value;
};

struct DND_ItemeffectAbilityBoost : DND_Itemeffect {
	DND_AbilityType type;
	int8_t value;
};

struct DND_ItemeffectComplex : DND_Itemeffect {
	std::string description;
};

struct DND_Item {
	std::string name;
	uint32_t amount;
	double weight;
	DND_Itemeffect* pNext;
};
