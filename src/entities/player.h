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
#define MAX_PLAYER_VELOCITY_Y 1.f
#define MIN_PLAYER_VELOCITY 0.1f
#define MAX_OVERLAP_LEN 12
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

  float delay;
  float animation;

  Vector2 position;
  Vector2 velocity;

  w_playerstate state;
} w_player;

w_player *create_player(unsigned int x);
void destroy_player(w_player *player);

void animate_player(w_player *player, float dt, bool should_walk);
void update_player_input(w_player *player, w_keyboard *keyboard);
void update_player_velocity(w_player *player);

Vector2 get_player_center(w_player *player);
Vector2 get_camera_target_player(w_player *player, Camera2D *camera);
void check_player_collision_vel(w_player *player, w_chunkview *view);
