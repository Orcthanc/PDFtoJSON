#pragma once

#include "Item.h"

#include <string>
#include <vector>

struct DND_Inventory {
	std::vector<DND_Item> items;

	uint32_t platinum_pieces;
	uint32_t gold_pieces;
	uint32_t electrum_pieces;
	uint32_t silver_pieces;
	uint32_t copper_pieces;
};

struct DND_Equipment {
	DND_Item head;
	DND_Item neck;
	DND_Item chest;
	DND_Item gauntlet;
	DND_Item back;
	DND_Item ring1;
	DND_Item ring2;
	DND_Item belt;
	DND_Item boot;

	std::vector<DND_Item> weapons;

	std::vector<DND_Item> additional_equipment;
};
