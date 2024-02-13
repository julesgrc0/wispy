#include "break.h"

w_blockbreaker *create_blockbreaker(w_state *state, w_chunkview *chunk_view,
                                    Camera2D *camera) {
  w_blockbreaker *bb = malloc(sizeof(w_blockbreaker));
  if (bb == NULL) {
    return NULL;
  }
  memset(bb, 0, sizeof(w_blockbreaker));

  bb->textures = malloc(sizeof(Texture) * BREAKER_STAGES);
  for (size_t i = 0; i < BREAKER_STAGES; i++) {
    bb->textures[i] =
        get_texture_by_id(state, (char *)TextFormat("blocks\\break_%i.png", i));
  }

  bb->time = 0;
  bb->stage = 0;
  bb->position = VEC_ZERO;
  bb->chunk_view = chunk_view;
  bb->camera = camera;

  return bb;
}

w_breakstate update_blockbreaker(w_blockbreaker *bb, w_player *player,
                                 float dt) {
  Vector2 mouse = get_mouse_block_center(bb->camera);
  if (Vector2Distance(mouse, get_player_center(player)) >= BREAKER_DISTANCE ||
      !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    bb->time = 0;
    return BS_NONE;
  }

  Vector2 bmouse = vec_block_round(mouse);
  w_block *block = get_chunkview_block(bb->chunk_view, bmouse);
  if (block == NULL || block->type == BLOCK_AIR) {
    bb->time = 0;
    return BS_NONE;
  }

  if (bb->time > 0) {
    if (!Vector2Equals(bb->position, bmouse)) {
      bb->time = 0;
      return false;
    }
    if ((player->src.width > 0 && bmouse.x < player->position.x) ||
        (player->src.width < 0 && bmouse.x > player->position.x)) {
      player->src.width = -player->src.width;
    }

    bb->time -= dt;
    bb->stage =
        (BREAKER_STAGES - 1) - ((bb->time) / (BREAKER_TIME / BREAKER_STAGES));

    if (bb->stage >= BREAKER_STAGES - 1 || bb->time <= 0) {
      bb->time = 0;
      bb->stage = 0;
      if (block != NULL && block->type != BLOCK_AIR) {
        block->type = BLOCK_AIR;
      }
      return BS_BROKEN;
    }

    return BS_BREAKING;
  }
  bb->time = BREAKER_TIME;
  bb->stage = BREAKER_STAGES - 1;
  bb->position = bmouse;
  return BS_NONE;
}

void draw_blockbreaker(w_blockbreaker *bb) {
  DrawTexturePro(bb->textures[bb->stage], CUBE_SRC_RECT,
                 (Rectangle){.x = bb->position.x,
                             .y = bb->position.y,
                             .width = CUBE_W,
                             .height = CUBE_H},
                 VEC_ZERO, 0, Fade(WHITE, 0.5));
}

void destroy_blockbreaker(w_blockbreaker *bb) {
  free(bb->textures);
  free(bb);
}
