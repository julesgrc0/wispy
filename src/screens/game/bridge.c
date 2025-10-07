#include "bridge.h"

w_bridge *create_bridge(w_config* cfg) {
  w_bridge *td = malloc(sizeof(w_bridge));
  if (td == NULL) {
    LOG("failed to allocate memory for bridge");
    return NULL;
  }
  memset(td, 0, sizeof(w_bridge));

  td->terrain = create_terrain(CHUNK_GROUP_MID_LEN);
  if (td->terrain == NULL) {
    destroy_bridge(td);
    return NULL;
  }

  td->player = create_player(td->terrain->group->position);
  if (td->player == NULL) {
    destroy_bridge(td);
    return NULL;
  }

  td->camera = create_camera(0, 0);
  if (td->camera == NULL) {
    destroy_bridge(td);
    return NULL;
  }
  set_camera_center(td->camera, get_player_center(td->player));
#if defined(WISPY_ANDROID)
  td->camera->target_position = td->camera->position;
#else
  td->camera->target_position = get_camera_vec(td->camera);
#endif

  td->ctrl = create_controls(cfg);
  if (td->ctrl == NULL) {
    destroy_bridge(td);
    return NULL;
  }

  td->is_active = true;
  td->force_update = true;

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX) || defined(WISPY_MACOS)

  update_bridge(td);

#if defined(WISPY_WINDOWS)
  QueryPerformanceFrequency(&td->time_frequency);
  QueryPerformanceCounter(&td->time_start);

  td->handle = CreateThread(NULL, 0, &thread_bridge, td, 0, NULL);
  if (td->handle == INVALID_HANDLE_VALUE)
#else
  clock_gettime(CLOCK_MONOTONIC, &td->time_start);

  if (pthread_create(&td->handle, NULL, &thread_bridge, td) != 0)
#endif
  {
    LOG("failed to create bridge thread");
    destroy_bridge(td);
    return NULL;
  }
#endif

  LOG("bridge thread created");
  return td;
}

void destroy_bridge(w_bridge *td) {

  LOG("destroying bridge thread");

  td->is_active = false;
#if defined(WISPY_WINDOWS)
  if (td->handle != INVALID_HANDLE_VALUE && td->handle != NULL) {
    WaitForSingleObject(td->handle, INFINITE);
  }
#elif defined(WISPY_LINUX)
  if (td->handle != 0) {
    pthread_join(td->handle, NULL);
  }
#endif

  destroy_controls(td->ctrl);
  destroy_terrain(td->terrain);
  destroy_player(td->player);
  destroy_camera(td->camera);

  sfree(td);
}

void physics_update_bridge(w_bridge *td) {

  check_player_collision_vel(td->player, td->terrain->view);
  Vector2 next_position =
      Vector2Scale(td->player->velocity, PHYSICS_TICK * PLAYER_SPEED);

  update_player_input(td->player, td->ctrl);
  update_player_velocity(td->player);

  td->player->position.x += next_position.x;
  td->player->position.y += next_position.y;

  Vector2 player_center = get_player_center(td->player);
  td->camera->target_position =
      VEC(player_center.x - RENDER_W / 2, player_center.y - RENDER_H / 2);

  animate_player(td->player, td->ctrl->left || td->ctrl->right);
  clear_controls(td->ctrl);
}

void update_bridge(w_bridge *td) {
  if (td->ctrl->key != 0 ||
#if defined(WISPY_ANDROID)
      !Vector2Equals(td->camera->target_position, td->camera->position) ||
#else
      !Vector2Equals(td->camera->target_position, get_camera_vec(td->camera)) ||
#endif
      td->force_update) {

    if (!update_chunkview(td->terrain->view, td->terrain->group, td->camera,
#if defined(WISPY_ANDROID)
                          update_renderblock
#else
                          update_renderblock_threadsafe
#endif
                          )) {
      LOG("failed to update chunk view");
      td->is_active = false;
      return;
    }
    update_chunkview_lighting(td->terrain->view, get_player_center(td->player),
                              RENDER_CUBE_COUNT * CUBE_W);
    td->force_update = false;
  }

#if defined(WISPY_WINDOWS)
  QueryPerformanceCounter(&td->time_end);
  if (td->time_end.QuadPart - td->time_start.QuadPart >=
      td->time_frequency.QuadPart * PHYSICS_TICK) {
    QueryPerformanceCounter(&td->time_start);
#elif defined(WISPY_LINUX)
  clock_gettime(CLOCK_MONOTONIC, &td->time_end);
  if ((td->time_end.tv_sec - td->time_start.tv_sec) +
          (td->time_end.tv_nsec - td->time_start.tv_nsec) / 1000000000.0 >=
      PHYSICS_TICK) {
    clock_gettime(CLOCK_MONOTONIC, &td->time_start);
#else
  {
#endif
    physics_update_bridge(td);
  }
}

#if defined(WISPY_WINDOWS)
int WINAPI thread_bridge(PVOID arg)
#else
void *thread_bridge(void *arg)
#endif
{
  LOG("starting bridge thread");
  if (!arg) {
#if defined(WISPY_WINDOWS)
    return EXIT_FAILURE;
#else
    return NULL;
#endif
  }

  w_bridge *td = arg;
  do {
    update_bridge(td);
  } while (td->is_active);

  LOG("exiting bridge thread");
#if defined(WISPY_WINDOWS)
  return EXIT_SUCCESS;
#else
  return NULL;
#endif
}
