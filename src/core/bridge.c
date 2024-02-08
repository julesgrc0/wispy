#include "bridge.h"

w_bridge *create_bridge() {
  w_bridge *td = malloc(sizeof(w_bridge));
  if (td == NULL) {
    LOG("failed to allocate memory for bridge");
    return NULL;
  }
  memset(td, 0, sizeof(w_bridge));

  td->chunk_group = create_chunkgroup(CHUNK_GROUP_MID_LEN);
  if (td->chunk_group == NULL) {
    destroy_bridge(td);
    return NULL;
  }
  td->chunk_view = create_chunkview(td->chunk_group->chunks[0]);
  if (td->chunk_view == NULL) {
    LOG("failed to allocate memory for chunk view");
    destroy_bridge(td);
    return NULL;
  }

  td->player = malloc(sizeof(w_player));
  if (td->player == NULL) {
    LOG("failed to allocate memory for player");
    destroy_bridge(td);
    return NULL;
  }
  memset(td->player, 0, sizeof(w_player));
  td->player->src = PLAYER_SRC_RECT;
  td->player->dst = (Rectangle){.x = (RENDER_W - CUBE_W) / 2.f,
                                .y = (RENDER_H - CUBE_H * 2) / 2.f,
                                .width = CUBE_W * 0.7f, // adjust player size
                                .height = CUBE_H * 2};
  td->player->position = (Vector2){
      .x = (td->chunk_group->position + CHUNK_GROUP_MID_LEN) * CHUNK_W * CUBE_W,
      .y = CHUNK_MID_H * CUBE_H};

  td->camera = malloc(sizeof(Camera2D));
  if (td->camera == NULL) {
    LOG("failed to allocate memory for camera");
    destroy_bridge(td);
    return NULL;
  }
  memset(td->camera, 0, sizeof(Camera2D));
  td->camera->zoom = 1.0f;
  td->camera->target = center_camera_on_object(
      td->camera, (Rectangle){.x = td->player->position.x,
                              .y = td->player->position.y,
                              .width = td->player->dst.width,
                              .height = td->player->dst.height});
  td->camera_target = td->camera->target;

  td->keyboard = malloc(sizeof(w_keyboard));
  if (td->keyboard == NULL) {
    LOG("failed to allocate memory for keyboard");
    destroy_bridge(td);
    return NULL;
  }
  memset(td->keyboard, 0, sizeof(w_keyboard));

  td->is_active = true;
#ifdef _WIN32
  td->handle = CreateThread(NULL, 0, &update_bridge, td, 0, NULL);
#else
  td->handle = pthread_create(&td->handle, NULL, &update_bridge, td);
#endif // _WIN32

  if (td->handle == INVALID_HANDLE_VALUE) {
    LOG("failed to create bridge thread");
    destroy_bridge(td);
    return NULL;
  }

  LOG("bridge thread created");
  return td;
}

void destroy_bridge(w_bridge *td) {
  LOG("destroying bridge thread");

  td->is_active = false;
  if (td->handle != INVALID_HANDLE_VALUE && td->handle != NULL) {
    WaitForSingleObject(td->handle, INFINITE);
  }

  destroy_chunkgroup(td->chunk_group);
  destroy_chunkview(td->chunk_view);

  sfree(td->camera);
  sfree(td->player);
  sfree(td->keyboard);

  sfree(td);
}

void physics_update(w_bridge *td) {
  if (td->keyboard->left) {
    if (td->player->src.width > 0) {
      td->player->src.width = -td->player->src.width;
    }
    td->player->velocity.x -= 1;
  } else if (td->keyboard->right) {
    if (td->player->src.width < 0) {
      td->player->src.width = -td->player->src.width;
    }
    td->player->velocity.x += 1;
  }

  if (td->keyboard->left || td->keyboard->right || !td->player->is_onground) {

    if (!td->player->is_onground) {
      td->player->velocity.y += 1;
    } else {
      td->player->velocity.y = 1;
      td->player->is_onground = false;
    }

    td->player->velocity = Vector2Normalize(td->player->velocity);
    td->player->position = Vector2Add(
        td->player->position,
        Vector2Scale(td->player->velocity, PLAYER_SPEED * PHYSICS_TICK));

    if (!td->player->is_onground) {
      Rectangle player_rect = {td->player->position.x, td->player->position.y,
                               td->player->dst.width, td->player->dst.height};
      for (size_t i = 0; i < td->chunk_view->len; i++) {

        Rectangle block_rect = td->chunk_view->blocks[i].dst;
        if (CheckCollisionRecs(player_rect, block_rect)) {
          /*
          td->player->position = Vector2Subtract(
              td->player->position,
              Vector2Scale(td->player->velocity, PLAYER_SPEED * PHYSICS_TICK));
          */

          if (td->player->position.y + td->player->dst.height >= block_rect.y) {
            td->player->is_onground = true;
            td->player->position.y = block_rect.y - td->player->dst.height;
            td->player->velocity.y = 0;
          }
          if (td->player->position.x + td->player->dst.width >= block_rect.x &&
              td->player->position.x <= block_rect.x + block_rect.width) {
            td->player->velocity.x = 0;
          }
          break;
        }
      }
    }

    td->camera_target = center_camera_on_object(
        td->camera, (Rectangle){.x = td->player->position.x,
                                .y = td->player->position.y,
                                .width = td->player->dst.width,
                                .height = td->player->dst.height});
  }

  td->player->animation += PHYSICS_TICK;
  td->player->velocity = Vector2Scale(td->player->velocity, 0.9f);
  if (abs(td->player->velocity.x) > 0.1f || td->keyboard->key != 0) {
    if (td->player->animation > PHYSICS_TICK * 2) {
      td->player->animation = 0;
      td->player->state = (td->player->state == P_WALK_1) ? P_WALK_2 : P_WALK_1;
    }
  } else {
    if (td->player->animation > PHYSICS_TICK * 12) {
      td->player->animation = 0;
      td->player->state = (td->player->state == P_IDLE_1) ? P_IDLE_2 : P_IDLE_1;
    }
  }
  clear_keyboard(td->keyboard);
}

#ifdef _WIN32
int WINAPI update_bridge(PVOID arg)
#else
void *update_bridge(void *arg)
#endif
{
  LOG("starting bridge thread");
  w_bridge *td = arg;
  LARGE_INTEGER time_start, time_end, time_frequency;

  QueryPerformanceFrequency(&time_frequency);
  QueryPerformanceCounter(&time_start);

  update_chunkview(td->chunk_view, td->chunk_group,
                   get_camera_view(td->camera));

  while (td->is_active) {

    if (td->keyboard->key != 0 || td->camera->target.x != td->camera_target.x ||
        td->camera->target.y != td->camera_target.y) {

      if (!update_chunkview(td->chunk_view, td->chunk_group,
                            get_camera_view(td->camera))) {
        LOG("failed to update chunk view");
        td->is_active = false;
        return EXIT_FAILURE;
      }
    }

    QueryPerformanceCounter(&time_end);
    if (time_end.QuadPart - time_start.QuadPart <
        time_frequency.QuadPart * PHYSICS_TICK)
      continue;
    QueryPerformanceCounter(&time_start);
    physics_update(td);
  }

  LOG("exiting bridge thread");
  return EXIT_SUCCESS;
}
