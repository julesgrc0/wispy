#include "player.h"

w_player *create_player(unsigned int x) {

  w_player *player = malloc(sizeof(w_player));
  if (player == NULL) {
    LOG("failed to allocate memory for player");
    return NULL;
  }
  memset(player, 0, sizeof(w_player));
  player->src = PLAYER_SRC_RECT;
  player->dst = (Rectangle){.x = (RENDER_W - CUBE_W) / 2.f,
                            .y = (RENDER_H - CUBE_H * 2) / 2.f,
                            .width = CUBE_W * 0.7f, // adjust player size
                            .height = CUBE_H * 2};
  player->position =
      (Vector2){.x = (x + CHUNK_GROUP_MID_LEN) * CHUNK_W * CUBE_W,
                .y = CHUNK_MID_H * CUBE_H};
  return player;
}

void destroy_player(w_player *player) {
  if (player == NULL)
    return;
  free(player);
}

void animate_player(w_player *player, float dt, bool should_walk) {
  player->animation += dt;

  if (should_walk || abs(player->velocity.x) > 0) {
    if (player->animation > dt * 2) {
      player->animation = 0;
      player->state = (player->state == P_WALK_1) ? P_WALK_2 : P_WALK_1;
    }
  } else {
    if (player->animation > dt * 12) {
      player->animation = 0;
      player->state = (player->state == P_IDLE_1) ? P_IDLE_2 : P_IDLE_1;
    }
  }
}

void update_player_input(w_player *player, w_keyboard *keyboard) {

  if (keyboard->left) {
    if (player->src.width > 0) {
      player->src.width = -player->src.width;
    }
    player->velocity.x -= 1;
  } else if (keyboard->right) {
    if (player->src.width < 0) {
      player->src.width = -player->src.width;
    }
    player->velocity.x += 1;
  }
  /*


    if (keyboard->jump && player->collision.bottom) {
      player->velocity.y -= 10;
    } else if (!player->collision.bottom) {
      player->velocity.y += 1;
    }
    */
}

Vector2 get_camera_target_player(w_player *player, Camera2D *camera) {

  return center_camera_on_object(camera,
                                 (Rectangle){.x = player->position.x,
                                             .y = player->position.y,
                                             .width = player->dst.width,
                                             .height = player->dst.height});
}
