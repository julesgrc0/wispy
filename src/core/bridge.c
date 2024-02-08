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
  td->player->collision = (w_collision){0};

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
  update_player_input(td->player, td->keyboard);

  if (td->player->velocity.x != 0 || td->player->velocity.y != 0) {

    /*
    td->player->velocity = Vector2Normalize(td->player->velocity);
    */
    td->player->velocity.x = Clamp(
        td->player->velocity.x, -MAX_PLAYER_VELOCITY_X, MAX_PLAYER_VELOCITY_X);
    td->player->velocity.y = Clamp(
        td->player->velocity.y, -MAX_PLAYER_VELOCITY_Y, MAX_PLAYER_VELOCITY_Y);

    Vector2 next_position = Vector2Add(
        td->player->position,
        Vector2Scale(td->player->velocity, PLAYER_SPEED * PHYSICS_TICK));
    td->player->velocity = Vector2Scale(td->player->velocity, PLAYER_FRICTION);

    Rectangle player_rect = (Rectangle){.x = next_position.x,
                                        .y = next_position.y,
                                        .width = td->player->dst.width,
                                        .height = td->player->dst.height};

    td->player->collision = (w_collision){0};
    unsigned int count = 0;
    for (size_t i = 0; i < td->chunk_view->len; i++) {
      Rectangle block_rect = td->chunk_view->blocks[i].dst;

      if (CheckCollisionRecs(player_rect, block_rect)) {

        // TODO: implement collision detection
        //
        //
        //  check if the collision is on left:
        //  if left -> set stop on left
        //  => stop velocity x
        //  => set collision at left(true) and set right(false)
        //  => set player position to be the right side of the block

        // etc...

        count++;
        if (count >= 3) {
          break;
        }
      }
    }

    if (td->player->collision.all == 0) {
      td->player->position = next_position;
    }
    td->camera_target = get_camera_target_player(td->player, td->camera);

    LOG("velocity: %f, %f", td->player->velocity.x, td->player->velocity.y);
  }

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
