#pragma once
#include "../stdafx.h"

typedef enum BlockTypes {
	B_NONE = 0,
	B_DIRT,
	B_GRASS,
	B_LEAVES,
	B_LOG,
	B_SAND,
	B_STONE
} BlockTypes;

typedef struct Block {
	unsigned int x : 4;   // 4 bits 0-9 (2^4)
	unsigned int y : 8;   // unsigned char

	bool interactable;
	BlockTypes type;
} Block;


