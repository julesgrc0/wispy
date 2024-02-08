#pragma once
#include "../core/state.h"
#include "../stdafx.h"
#include "../terrain/chunk_view.h"

#define PLAYER_SPEED 1000.f
#define PLAYER_SRC_RECT                                                        \
  (Rectangle) { 0, 0, 8, 16 }

typedef enum w_playerstate {
  P_IDLE_1,
  P_IDLE_2,
  P_WALK_1,
  P_WALK_2
} w_playerstate;

typedef struct w_player {
  Rectangle src;
  Rectangle dst;

  Vector2 position;
  Vector2 velocity;

  float delay;
  float animation;

  unsigned int nearest_y;
  bool is_onground;
  w_playerstate state;
} w_player;
