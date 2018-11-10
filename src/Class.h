#pragma once

#include "Stats.h"
#include "Spell.h"
#include "DruidForm.h"
#include "Companion.h"

#include <string>
#include <vector>

enum DND_ClassExtType {
	NoExt,
	SpellcasterShort,
	SpellcasterLong,
	DruidForms,
	RangerCompanion
};

struct DND_ClassExt {
	DND_ClassExtType sType;
	DND_ClassExt* pNext;
	DND_ClassExtType pNextType;
};

struct DND_ClassExtSpellcasterShort : DND_ClassExt {
	std::string spell_class_name;
	std::string point_pool_name;
	uint8_t point_pool_total;
	uint8_t point_pool_use;
	DND_Stat spell_ability;
	uint8_t spell_save_dc;
	int8_t spell_atk_bonus;
	std::vector<DND_spell> spells;
	std::vector<uint8_t> slots_total;
	std::vector<uint8_t> slots_used;
};

struct DND_ClassExtSpellcasterLong : DND_ClassExtSpellcasterShort {};

struct DND_ClassExtDruid : DND_ClassExt {
	std::vector<DruidForm> forms;
};

struct DND_ClassRangerCompanion : DND_ClassExt {
	std::vector<Companion> companions;
};

struct DND_Class {
	std::string name;
	uint8_t level;
	uint8_t hit_die_type;
	DND_ClassExtType pNextType;
	DND_ClassExt pNext;
};
