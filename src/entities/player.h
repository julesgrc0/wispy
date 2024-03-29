#pragma once
#include "../core/controls.h"
#include "../core/state.h"
#include "../stdafx.h"
#include "../terrain/chunk_view.h"

#define PLAYER_ANIMTATION_TIME(x) ((x * (1 / PHYSICS_TICK)) / 30)
#define PLAYER_ANIMATION_IDLE PLAYER_ANIMTATION_TIME(12)
#define PLAYER_ANIMATION_WALK PLAYER_ANIMTATION_TIME(4)

#define PLAYER_SPEED 250.f
#define PLAYER_JUMP 2.f
#define PLAYER_JUMP_DURATION PLAYER_ANIMTATION_TIME(8)
#define PLAYER_JUMP_DELAY PLAYER_ANIMTATION_TIME(4)
#define PLAYER_FRICTION 0.8f

#define MAX_PLAYER_VELOCITY_X 3.f
#define MAX_PLAYER_SLOW_VELOCITY_X 1.f
#define MAX_PLAYER_VELOCITY_Y 6.f
#define MIN_PLAYER_VELOCITY 0.1f

#define PLAYER_SRC_RECT                                                        \
  (Rectangle) { 0, 0, 8, 16 }

typedef enum w_playerstate {
  P_IDLE_1,
  P_IDLE_2,
  P_WALK_1,
  P_WALK_2,
  P_FALL_1,
  P_FALL_2,
  P_CLIMB_1,
  P_CLIMB_2,
} w_playerstate;

typedef enum w_playerjump {
  PJ_GROUD,
  PJ_JUMP,
  PJ_FALL,
} w_playerjump;

typedef struct w_player {
  Rectangle src;
  Rectangle dst;

  float duration;
  float delay;
  float animation;

  Vector2 position;
  Vector2 velocity;

  w_playerjump jump;
  w_playerstate state;
} w_player;

w_player *create_player(unsigned int x);
void destroy_player(w_player *player);

void animate_player(w_player *player, bool should_walk);
void update_player_input(w_player *player, w_controls *ctrl);
void update_player_velocity(w_player *player);

Vector2 get_player_center(w_player *player);
void check_player_collision_vel(w_player *player, w_chunkview *view);
