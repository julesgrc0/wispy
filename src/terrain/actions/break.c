#include "break.h"

w_blockbreaker *create_blockbreaker(w_state *state, w_chunkview *chunk_view,
                                    w_camera *camera) {
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

Vector2 get_mouse_block(w_camera *camera) {
  Vector2 mouse = Vector2Add(VEC(FORMAT_W(GetMouseX()), FORMAT_H(GetMouseY())),
#if defined(WISPY_ANDROID)
                             camera->position);
#else
                             get_camera_vec(camera));
#endif

  return Vector2Subtract(mouse, VEC((CUBE_W / 2), (CUBE_H / 2)));
}

w_breakstate update_blockbreaker(w_blockbreaker *bb, w_controls *ctrl,
                                 w_player *player, float dt) {
#if defined(WISPY_ANDROID)
  Vector2 mouse = vec_block_round(
      Vector2Add(player->position, Vector2Scale(ctrl->joystick, CUBE_W)));
  if (!ctrl->is_breaking) {
    bb->time = 0;
    return BS_NONE;
  }
#else
  Vector2 mouse = vec_block_round(get_mouse_block(bb->camera));
  if (Vector2Distance(mouse, get_player_center(player)) >= BREAKER_DISTANCE ||
      !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    bb->time = 0;
    return BS_NONE;
  }
#endif

  w_block *block = get_chunkview_block(bb->chunk_view, mouse);
  if (block == NULL || block->type == BLOCK_AIR || block->is_background) {
    bb->time = 0;
    return BS_NONE;
  }

  if (bb->time > 0) {

    if (!Vector2Equals(bb->position, mouse)) {
      bb->time = 0;
      return false;
    }

    if ((player->src.width > 0 && mouse.x < player->position.x) ||
        (player->src.width < 0 && mouse.x > player->position.x)) {
      player->src.width = -player->src.width;
    }

    bb->time -= dt;
    bb->stage = (int)((BREAKER_STAGES - 1) -
                      ((bb->time) / (BREAKER_TIME / BREAKER_STAGES)));

    if (bb->stage >= BREAKER_STAGES - 1 || bb->time <= 0) {
      bb->time = 0;
      bb->stage = 0;
      if (block != NULL && block->type != BLOCK_AIR && !block->is_background) {
        block->is_background = true;
      }
      return BS_BROKEN;
    }

    return BS_BREAKING;
  }
  bb->time = BREAKER_TIME;
  bb->stage = BREAKER_STAGES - 1;
  bb->position = mouse;
  return BS_NONE;
}

void draw_blockbreaker(w_blockbreaker *bb, w_camera *camera) {
  DrawTexturePro(bb->textures[bb->stage], CUBE_SRC_RECT,
#if defined(WISPY_ANDROID)
                 RECT(bb->position.x - camera->position.x,
                      bb->position.y - camera->position.y, CUBE_W, CUBE_H),
#else
                 RECT(bb->position.x, bb->position.y, CUBE_W, CUBE_H),
#endif
                 VEC_ZERO, 0, Fade(WHITE, 0.5));
}

void destroy_blockbreaker(w_blockbreaker *bb) {
  free(bb->textures);
  free(bb);
}
