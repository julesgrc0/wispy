#pragma once
#include "../stdafx.h"

typedef enum PlayerState {
	NONE = 0
} PlayerState;

typedef struct Player {
	int x;
	unsigned int y : 8;

	PlayerState state;
} Player;