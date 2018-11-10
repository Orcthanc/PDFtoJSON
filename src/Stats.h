#pragma once

#include <stdint.h>
#include <string>

enum DND_stat_names {
	#define STAT( A, B ) A,
	#include "stats.inl"
};

struct DND_Stat {
	DND_stat_names stat_type;
	std::string stat_name;
	uint8_t ability_score_base;
	int8_t ability_score_bonus;
	int8_t ability_score_mod;

	DND_Stat( DND_stat_names stat, std::string name ){
		this->stat_type = stat;
		this->stat_name = name;
	}
};

struct DND_Stats {
	#define STAT( A, B ) DND_Stat A = DND_Stat( DND_stat_names::A, #B );
	#include "stats.inl"
};
