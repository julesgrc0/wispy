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
    destroy_bridge(td);
    return NULL;
  }

  td->next_view = create_chunkview(td->chunk_group->chunks[0]);
  if (td->next_view == NULL) {
    destroy_bridge(td);
    return NULL;
  }

  td->player = create_player(td->chunk_group->position);
  if (td->player == NULL) {
    destroy_bridge(td);
    return NULL;
  }

  td->camera = malloc(sizeof(Camera2D));
  if (td->camera == NULL) {
    LOG("failed to allocate memory for camera");
    destroy_bridge(td);
    return NULL;
  }
  memset(td->camera, 0, sizeof(Camera2D));
  td->camera->zoom = 1.0f;
  td->camera->target = get_camera_target_player(td->player, td->camera);
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
  destroy_chunkview(td->next_view);
  destroy_player(td->player);

  sfree(td->camera);
  sfree(td->keyboard);
  sfree(td);
}

void physics_update(w_bridge *td) {
  update_player_input(td->player, td->keyboard);
  update_player_velocity(td->player);

  Vector2 next_position =
      Vector2Scale(td->player->velocity, PLAYER_SPEED * PHYSICS_TICK);
  Rectangle player_rect = {.x = td->player->position.x,
                           .y = td->player->position.y,
                           .width = td->player->dst.width,
                           .height = td->player->dst.height};

  // TODO: find a better physics solution
  if (next_position.y != 0) {

    player_rect.y += next_position.y;
    update_chunkview(td->next_view, td->chunk_group,
                     (Rectangle){.x = player_rect.x,
                                 .y = player_rect.y,
                                 .width = RENDER_W,
                                 .height = RENDER_H});

    Rectangle overlap =
        get_player_collision_overlap(player_rect, td->next_view);
    if (overlap.y != 0) {
      td->player->velocity.y = 0;

      player_rect.y = round((overlap.y - player_rect.height) / CUBE_H) * CUBE_H;
      td->player->is_onground = true;
    } else {
      td->player->is_onground = false;
    }
  }
  if (next_position.x != 0) {
    player_rect.x += next_position.x;
    update_chunkview(td->next_view, td->chunk_group,
                     (Rectangle){.x = player_rect.x,
                                 .y = player_rect.y,
                                 .width = RENDER_W,
                                 .height = RENDER_H});

    Rectangle overlap =
        get_player_collision_overlap(player_rect, td->next_view);
    if (overlap.x != 0 && overlap.y == 0) {

      player_rect.x = overlap.x;
      td->player->velocity.x = 0;

    } else if (overlap.x != 0 && overlap.y != 0) {

      td->player->is_onground = false;
      player_rect.x =
          ((int)((td->player->position.x) / CUBE_W)) * CUBE_W +
          (td->player->velocity.x > 0 ? (CUBE_W - player_rect.width) : 0);
      td->player->velocity.x = 0;
    }
  }

  td->player->position.x = player_rect.x;
  td->player->position.y = player_rect.y;

  td->camera_target = get_camera_target_player(td->player, td->camera);
  animate_player(td->player, PHYSICS_TICK, td->keyboard->key != 0);
  clear_keyboard(td->keyboard);
}

#ifdef _WIN32
int WINAPI update_bridge(PVOID arg)
#else
void *update_bridge(void *arg)
#endif
{
  LOG("starting bridge thread");
  if (!arg) {
    return EXIT_FAILURE;
  }

  w_bridge *td = arg;

#ifdef _WIN32
  LARGE_INTEGER time_start, time_end, time_frequency;

  QueryPerformanceFrequency(&time_frequency);
  QueryPerformanceCounter(&time_start);
#else
  struct timespec time_start, time_end;
  time_start.tv_sec = 0;
  time_start.tv_nsec = 0;
  time_end.tv_sec = 0;
  time_end.tv_nsec = 0;
#endif // _WIN32

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
      update_chunkview_lighting(
          td->chunk_view,
          (Vector2){td->player->position.x + td->player->dst.width / 2,
                    td->player->position.y + td->player->dst.height / 2});
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
