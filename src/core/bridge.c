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
  destroy_player(td->player);

  sfree(td->camera);
  sfree(td->keyboard);
  sfree(td);
}

void physics_update(w_bridge *td) {
  // update_player_input(td->player, td->keyboard);
  /*
  if (td->keyboard->key == 0 && td->player->velocity.x == 0 &&
      td->player->velocity.y == 0 && td->player->collision.bottom) {

    LOG("static");
    animate_player(td->player, PHYSICS_TICK, td->keyboard->key != 0);
    return;
  }

  td->player->velocity.y += 1;

  update_player_velocity(td->player, PHYSICS_TICK);

  Rectangle player_rect = (Rectangle){.x = td->player->position.x,
                                      .y = td->player->position.y,
                                      .width = td->player->dst.width,
                                      .height = td->player->dst.height};

  td->player->collision.all = 0;
  Rectangle collisions[4] = {0};
  size_t len = 0;

  for (size_t i = 0; i < td->chunk_view->len; i++) {
    Rectangle block_rect = td->chunk_view->blocks[i].dst;

    if (CheckCollisionRecs(player_rect, block_rect)) {
      collisions[len] = block_rect;
      len++;
      if (len >= 4) {
        break;
      }
    }
  }
  if (len > 0) {
    td->player->velocity = VEC_ZERO;
  } else {
    td->player->position = Vector2Add(
        td->player->position,
        Vector2Scale(td->player->velocity, PLAYER_SPEED * PHYSICS_TICK));
  }

  if (td->keyboard->left) {
    td->player->body->velocity.x = 1;
  } else if (td->keyboard->right) {
    td->player->body->velocity.x = -1;
  }

  if (td->keyboard->jump) {
    td->player->body->velocity.y = -4;
  }

  unsigned int around_count = 0;
  for (size_t i = 0; i < td->chunk_view->len; i++) {
    Vector2 block_center =
        (Vector2){.x = td->chunk_view->blocks[i].dst.x + CUBE_W / 2,
                  .y = td->chunk_view->blocks[i].dst.y + CUBE_H / 2};
    if (abs(Vector2Distance(td->player->body->position, block_center)) <
        CUBE_W * 2) {

      around_count++;
      if (around_count >= 12) {
        break;
      }
    }
  }
  */

  Vector2 player_center = {td->player->position.x + td->player->dst.width / 2.f,
                           td->player->position.y +
                               td->player->dst.height / 2.f};
  Vector2 block_center = {0};
  Rectangle blocks[12] = {0};
  size_t len = 0;
  for (size_t i = 0; i < td->chunk_view->len; i++) {

    block_center.x = td->chunk_view->blocks[i].dst.x + CUBE_W / 2;
    block_center.y = td->chunk_view->blocks[i].dst.y + CUBE_H / 2;

    if (abs(Vector2Distance(player_center, block_center)) < CUBE_W * 2) {
      blocks[len] = td->chunk_view->blocks[i].dst;
      len++;
      if (len >= 12) {
        break;
      }
    }
  }

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
