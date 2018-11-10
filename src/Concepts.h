#pragma once

#include <stdint.h>

struct DND_Speed {
	uint8_t runspeed;
	uint8_t climbspeed;
	uint8_t swimspeed;
	uint8_t flyspeed;
};

struct DND_Lifemanagement {
	uint8_t max_hp;
	int8_t current_hp;
	int8_t temporary_hp;
	uint8_t death_save_successes;
	uint8_t death_save_failures;
};

enum DND_DamageModifiersType {
	DMG_Immune,
	DMG_Resistance,
	DMG_Vulnerable,
};

struct DND_DamageModifiers {
	#define DAMAGE_TYPE( name ) DND_DamageModifiersType name;
};
