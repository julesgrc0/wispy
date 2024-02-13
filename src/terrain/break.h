#pragma once
#include "../core/state.h"
#include "../stdafx.h"
#include "chunk_view.h"

#define BREAKER_TIME .8f
#define BREAKER_STAGES 6
#define BREAKER_DISTANCE (3 * CUBE_W)

typedef enum w_breakstate {
  BS_NONE,
  BS_BREAKING,
  BS_BROKEN,
} w_breakstate;

typedef struct w_blockbreaker {
  float time;
  int stage;
  Vector2 position;

  w_chunkview *chunk_view;
  Camera2D *camera;

  Texture *textures;
} w_blockbreaker;

w_blockbreaker *create_blockbreaker(w_state *state, w_chunkview *chunk_view,
                                    Camera2D *camera);

w_breakstate update_blockbreaker(w_blockbreaker *bb, Vector2 player, float dt);

void draw_blockbreaker(w_blockbreaker *bb);

void destroy_blockbreaker(w_blockbreaker *bb);
