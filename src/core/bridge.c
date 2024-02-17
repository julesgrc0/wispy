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
  td->camera->zoom = 1.f;

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
  if (td->handle == INVALID_HANDLE_VALUE)
#else
  if (pthread_create(&td->handle, NULL, &update_bridge, td) != 0)
#endif // _WIN32
  {
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
#ifdef _WIN32
  if (td->handle != INVALID_HANDLE_VALUE && td->handle != NULL) {
    WaitForSingleObject(td->handle, INFINITE);
  }
#else
  if (td->handle != 0) {
    pthread_join(td->handle, NULL);
  }
#endif // _WIN32

  destroy_chunkgroup(td->chunk_group);
  destroy_chunkview(td->chunk_view);
  destroy_player(td->player);

  sfree(td->camera);
  sfree(td->keyboard);
  sfree(td);
}

void physics_update(w_bridge *td) {

  check_player_collision_vel(td->player, td->chunk_view);
  Vector2 next_position =
      Vector2Scale(td->player->velocity, PHYSICS_TICK * PLAYER_SPEED);

  update_player_input(td->player, td->keyboard);
  update_player_velocity(td->player);

  td->player->position.x += next_position.x;
  td->player->position.y += next_position.y;

  td->camera_target = get_camera_target_player(td->player, td->camera);
  animate_player(td->player, td->keyboard->left || td->keyboard->right);
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
#ifdef _WIN32
    return EXIT_FAILURE;
#else
    return NULL;
#endif
  }

  w_bridge *td = arg;

#ifdef _WIN32
  LARGE_INTEGER time_start, time_end, time_frequency;

  QueryPerformanceFrequency(&time_frequency);
  QueryPerformanceCounter(&time_start);
#else
  struct timespec time_start, time_end;
#endif // _WIN32

  update_chunkview(td->chunk_view, td->chunk_group,
                   get_camera_view(td->camera));

  update_chunkview_lighting(td->chunk_view, get_player_center(td->player),
                            RENDER_CUBE_COUNT * CUBE_W);
  do {

    if (td->keyboard->key != 0 || td->camera->target.x != td->camera_target.x ||
        td->camera->target.y != td->camera_target.y || td->force_update) {

      if (!update_chunkview(td->chunk_view, td->chunk_group,
                            get_camera_view(td->camera))) {
        LOG("failed to update chunk view");
        td->is_active = false;

#ifdef _WIN32
        return EXIT_FAILURE;
#else
        return NULL;
#endif
      }
      update_chunkview_lighting(td->chunk_view, get_player_center(td->player),
                                RENDER_CUBE_COUNT * CUBE_W);
      td->force_update = false;
    }

#ifdef _WIN32
    QueryPerformanceCounter(&time_end);
    if (time_end.QuadPart - time_start.QuadPart >=
        time_frequency.QuadPart * PHYSICS_TICK) {
      QueryPerformanceCounter(&time_start);
#elif __linux__
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    if ((time_end.tv_sec - time_start.tv_sec) +
            (time_end.tv_nsec - time_start.tv_nsec) / 1000000000 >=
        PHYSICS_TICK) {
      clock_gettime(CLOCK_MONOTONIC, &time_start);
#endif // _WIN32

      physics_update(td);
    }
  } while (td->is_active);

  LOG("exiting bridge thread");
#ifdef _WIN32
  return EXIT_SUCCESS;
#else
  return NULL;
#endif
}
