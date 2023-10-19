#pragma once
#include "../stdafx.h"

typedef enum BlockTypes {
	NONE = 0,
	
} BlockTypes;

typedef struct Block {
	short int x, y;
	bool interactable;
	BlockTypes type;
} Block;