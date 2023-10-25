#pragma once
#include "../stdafx.h"
#include "../physac.h"

#include "../core/state.h"
#include "../terrain/chunk.h"

#define PLAYER_VELOCITY 0.5

typedef enum PlayerState
{
	P_IDLE_1,
	P_IDLE_2,
	P_WALK_1,
	P_WALK_2
} PlayerState;

typedef struct Player
{
	Vector2 position;

	unsigned int direction : 1;
	PlayerState state;
} Player;

void update_player(Player*, PhysicsBody);
