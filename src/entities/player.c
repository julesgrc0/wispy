#include "player.h"

void animate_player(w_player *player, float dt, bool should_walk) {
  player->animation += dt;
  if (abs(player->velocity.x) > 0.01f || should_walk) {
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
  if (keyboard->left && !player->collision.left) {
    if (player->src.width > 0) {
      player->src.width = -player->src.width;
    }
    player->velocity.x -= 1;
  } else if (keyboard->right && !player->collision.right) {
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
