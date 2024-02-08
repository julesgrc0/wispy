#pragma once
#include "../core/keyboard.h"
#include "../core/state.h"
#include "../core/view.h"
#include "../stdafx.h"
#include "../terrain/chunk_view.h"

#define PLAYER_SPEED 1000.f
#define PLAYER_JUMP 10.f
#define PLAYER_FRICTION 0.8f
#define MAX_PLAYER_VELOCITY_X 1.f
#define MAX_PLAYER_VELOCITY_Y 10.f

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

  w_collision collision;
  w_playerstate state;
} w_player;

void animate_player(w_player *player, float dt, bool should_walk);
void update_player_input(w_player *player, w_keyboard *keyboard);
Vector2 get_camera_target_player(w_player *player, Camera2D *camera);
