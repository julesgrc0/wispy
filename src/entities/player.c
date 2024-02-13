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

void animate_player(w_player *player, bool should_walk) {
  player->animation += PHYSICS_TICK;

  if (should_walk || abs(player->velocity.x) > 0) {
    if (player->animation > PHYSICS_TICK * 4) {
      player->animation = 0;
      player->state = (player->state == P_WALK_1) ? P_WALK_2 : P_WALK_1;
    }
  } else {
    if (player->animation > PHYSICS_TICK * 24) {
      player->animation = 0;
      player->state = (player->state == P_IDLE_1) ? P_IDLE_2 : P_IDLE_1;
    }
  }
}

void update_player_input(w_player *player, w_keyboard *keyboard) {
  if (player->delay > 0) {
    player->delay--;
  }
  if (keyboard->jump && player->delay <= 0) {
    player->velocity.y -= PLAYER_JUMP;
    player->delay = (1 / PHYSICS_TICK);
  }

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
}

void update_player_velocity(w_player *player) {
  player->velocity.y += 1 / 2.f;

  player->velocity.x =
      Clamp(player->velocity.x, -MAX_PLAYER_VELOCITY_X, MAX_PLAYER_VELOCITY_X);

  player->velocity.y =
      Clamp(player->velocity.y, -PLAYER_JUMP, MAX_PLAYER_VELOCITY_Y);

  player->velocity = Vector2Scale(player->velocity, PLAYER_FRICTION);

  if (fabs(player->velocity.x) < 0.1f) {
    player->velocity.x = 0;
  }

  if (fabs(player->velocity.y) < 0.1f) {
    player->velocity.y = 0;
  }
}

Vector2 get_player_center(w_player *player) {
  return (Vector2){player->position.x + player->dst.width / 2,
                   player->position.y + player->dst.height / 2};
}

Vector2 get_camera_target_player(w_player *player, Camera2D *camera) {

  return center_camera_on_object(camera,
                                 (Rectangle){.x = player->position.x,
                                             .y = player->position.y,
                                             .width = player->dst.width,
                                             .height = player->dst.height});
}

void check_player_collision_vel(w_player *player, w_chunkview *view) {
  Rectangle next_velx = {.x = player->position.x +
                              player->velocity.x * PLAYER_SPEED * PHYSICS_TICK,
                         .y = player->position.y,
                         .width = player->dst.width,
                         .height = player->dst.height};
  Rectangle next_vely = {.x = player->position.x,
                         .y = player->position.y +
                              player->velocity.y * PLAYER_SPEED * PHYSICS_TICK,
                         .width = player->dst.width,
                         .height = player->dst.height};

  bool col_x = false;
  bool col_y = false;

  for (size_t i = 0; i < view->textures_len; i++) {
    Rectangle block = view->blocks[i].dst;
    if (!col_x && CheckCollisionRecs(block, next_velx)) {
      col_x = true;
      player->velocity.x = 0;

      float adjust_x = GetCollisionRec(block, next_velx).x - player->dst.width;
      if (adjust_x > player->position.x) {
        player->position.x = adjust_x;
      }

      if (col_y) {
        break;
      }
    }

    if (!col_y && CheckCollisionRecs(block, next_vely)) {

      player->velocity.y = 0;

      float adjust_y = GetCollisionRec(block, next_vely).y - player->dst.height;
      if (adjust_y > player->position.y) {

        player->position.y = adjust_y;
      }
    }
  }
}
