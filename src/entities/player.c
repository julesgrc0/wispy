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

  if (player->delay > 0) {
    player->delay--;
  }
  if (keyboard->jump && player->is_onground && player->delay <= 0) {
    player->velocity.y -= 5;
    player->delay = (1 / PHYSICS_TICK);
    player->is_onground = false;
  }

  if (keyboard->left) {

    if (player->src.width > 0) {
      player->src.width = -player->src.width;
    }
    player->velocity.x -= 1;
    player->is_onground = false;
  } else if (keyboard->right) {

    if (player->src.width < 0) {
      player->src.width = -player->src.width;
    }
    player->velocity.x += 1;
    player->is_onground = false;
  }
}

void update_player_velocity(w_player *player) {
  if (!player->is_onground) {
    player->velocity.y += 1;
  }

  player->velocity.x =
      Clamp(player->velocity.x, -MAX_PLAYER_VELOCITY_X, MAX_PLAYER_VELOCITY_X);

  player->velocity.y =
      Clamp(player->velocity.y, -MAX_PLAYER_VELOCITY_Y, MAX_PLAYER_VELOCITY_Y);

  player->velocity = Vector2Scale(player->velocity, PLAYER_FRICTION);

  if (fabs(player->velocity.x) < 0.1f) {
    player->velocity.x = 0;
  }

  if (fabs(player->velocity.y) < 0.1f) {
    player->velocity.y = 0;
  }
}

Vector2 get_camera_target_player(w_player *player, Camera2D *camera) {

  return center_camera_on_object(camera,
                                 (Rectangle){.x = player->position.x,
                                             .y = player->position.y,
                                             .width = player->dst.width,
                                             .height = player->dst.height});
}

Rectangle get_player_collision_overlap(Rectangle player, w_chunkview *view) {

  Rectangle overlap = {0};
  float min_dst = MAXUINT16;
  size_t col_count = 0;
  for (size_t i = 0; i < view->len; i++) {
    Rectangle block = view->blocks[i].dst;
    if (CheckCollisionRecs(block, player)) {
      Rectangle block_overlap = GetCollisionRec(block, player);

      float dist = Vector2Distance((Vector2){.x = block.x, .y = block.y},
                                   (Vector2){.x = player.x, .y = player.y});

      if (dist < min_dst) {
        min_dst = dist;
        overlap = block_overlap;
      }

      col_count++;
      if (col_count >= MAX_OVERLAP_LEN) {
        break;
      }
      /*
        Vector2 block_center = (Vector2){.x = block.x + block.width / 2,
                                       .y = block.y + block.height / 2};
      Vector2 player_center = (Vector2){.x = player.x + player.width / 2,
                                        .y = player.y + player.height / 2};

      float dist = Vector2Distance(block_center, player_center);
      if (dist < min_dst) {
        min_dst = dist;
        overlap = ;
      }
      */
    }
  }

  return overlap;
}
