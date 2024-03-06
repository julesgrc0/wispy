#pragma once
#include "../core/controls.h"
#include "../core/state.h"
#include "../core/utils/camera.h"
#include "../core/utils/smooth.h"
#include "../entities/player.h"
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
  w_camera *camera;

  Texture *textures;
} w_blockbreaker;

Vector2 get_mouse_block(w_camera *camera);

w_blockbreaker *create_blockbreaker(w_state *state, w_chunkview *chunk_view,
                                    w_camera *camera);

w_breakstate update_blockbreaker(w_blockbreaker *bb, w_controls *ctrl,
                                 w_player *player, float dt);

void draw_blockbreaker(w_blockbreaker *bb, w_camera* camera);

void destroy_blockbreaker(w_blockbreaker *bb);
