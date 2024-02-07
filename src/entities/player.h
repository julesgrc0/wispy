#pragma once
#include "../core/state.h"
#include "../stdafx.h"
#include "../terrain/chunk.h"

typedef enum w_playerstate {
  P_IDLE_1,
  P_IDLE_2,
  P_WALK_1,
  P_WALK_2
} w_playerstate;

typedef struct w_player {
  Rectangle box;

  Vector2 velocity;

  float delay;
  float animation;

  unsigned int onground : 1;
  unsigned int direction : 1;
  w_playerstate state;
} w_player;

void update_player(w_player *player, float velocity, float dt);
