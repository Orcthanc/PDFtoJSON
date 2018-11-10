#pragma once

#include <stdint.h>

#include <string>

static const uint8_t DND_SPELL_MATERIAL		= 1 << 0;
static const uint8_t DND_SPELL_SOMATIC			= 1 << 1;
static const uint8_t DND_SPELL_VERBAL			= 1 << 2;
static const uint8_t DND_SPELL_CONCENTRATION	= 1 << 3;
static const uint8_t DND_SPELL_RITUAL			= 1 << 4;
static const uint8_t DND_SPELL_REACTION		= 1 << 5;
static const uint8_t DND_SPELL_BONUS_ACTION	= 1 << 6;

struct DND_spell{
	uint8_t level;
	uint8_t flags;
	std::string name;
};
