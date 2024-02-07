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
  td->player->box =
      (Rectangle){.x = CHUNK_MID_H * CUBE_H,
                  .y = td->chunk_group->position * CHUNK_W * CUBE_W,
                  .width = CUBE_W,
                  .height = CUBE_H * 2};

  td->camera = malloc(sizeof(Camera2D));
  if (td->camera == NULL) {
    LOG("failed to allocate memory for camera");
    destroy_bridge(td);
    return NULL;
  }
  memset(td->camera, 0, sizeof(Camera2D));
  td->camera->zoom = 1.0f;
  td->camera->target = center_camera_on_object(td->camera, td->player->box);

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
  if (td->keyboard->up) {
    td->player->velocity.y -= 1;
  } else if (td->keyboard->down) {
    td->player->velocity.y += 1;
  }

  if (td->keyboard->left) {
    td->player->velocity.x -= 1;
  } else if (td->keyboard->right) {
    td->player->velocity.x += 1;
  }

  if (td->keyboard->space) {
    td->camera->zoom += 0.1f;
  } else if (td->keyboard->shift) {
    td->camera->zoom -= 0.1f;
  }

  if (td->keyboard->key != 0) {
    td->player->velocity = Vector2Normalize(td->player->velocity);
    Vector2 position = Vector2Scale(td->player->velocity, 1000 * PHYSICS_TICK);
    td->player->box.x += position.x;
    td->player->box.y += position.y;
  }

  td->player->velocity = Vector2Scale(td->player->velocity, 0.9f);
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
    if (td->keyboard->key != 0) {
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
