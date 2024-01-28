#pragma once
#include "../core/state.h"
#include "../stdafx.h"
#include "../terrain/chunk.h"

typedef enum PlayerState { P_IDLE_1, P_IDLE_2, P_WALK_1, P_WALK_2 } PlayerState;

typedef struct Player {
  Vector2 position;
  Vector2 velocity;

  float delay;
  float animation;

  unsigned int onground : 1;
  unsigned int direction : 1;
  PlayerState state;
} Player;

void update_player(Player *player, float velocity, float dt);
