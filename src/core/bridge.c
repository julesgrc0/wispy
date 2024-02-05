#include "bridge.h"

ThreadData *start_threadbridge(Player *player)
{
  ThreadData *td = malloc(sizeof(ThreadData));
  memset(td, 0, sizeof(ThreadData));

  td->request_update = false;
  td->request_swap = false;
  td->is_locked = false;
  td->is_active = true;

  QueryPerformanceFrequency(&td->time_frequency);
  QueryPerformanceCounter(&td->time_start);
  QueryPerformanceCounter(&td->time_end);

  td->chunkGroup = create_chunk_group(CHUNK_GROUP_MID_LEN);
  td->chunkView = create_chunk_view();

  td->camera = malloc(sizeof(Camera2D));
  memset(td->camera, 0, sizeof(Camera2D));

  td->camera->zoom = .5f;
  td->camera->target.y = CHUNK_MID_H * CUBE_H;
  td->camera->target.x = td->chunkGroup->position * CHUNK_W * CUBE_W;

  td->player = player;
  td->player->src = (Rectangle){0, 0, CUBE_W, CUBE_H * 2};
  td->player->box = (Rectangle){0, 0, td->player->src.width * 0.9, td->player->src.height * 0.9};

  td->handle = CreateThread(NULL, 0, &update_thread, td, 0, NULL);

  return td;
}

void stop_threadbridge(ThreadData *td)
{
  td->is_active = false;
  if (td->handle != INVALID_HANDLE_VALUE && td->handle != NULL)
  {
    WaitForSingleObject(td->handle, INFINITE);
  }

  for (unsigned int i = 0; i < CHUNK_GROUP_LEN; i++)
  {
    if (td->chunkGroup->chunks[i]->handle != INVALID_HANDLE_VALUE)
    {
      WaitForSingleObject(td->chunkGroup->chunks[i]->handle, INFINITE);
    }

    free(td->chunkGroup->chunks[i]);
  }
  free(td->chunkGroup);

  free(td->chunkView->blocks);
  free(td->chunkView);

  free(td->camera);
  free(td->player);

  free(td);
}

void physics_update(ThreadData *td)
{
  QueryPerformanceCounter(&td->time_end);
  if (td->time_end.QuadPart - td->time_start.QuadPart < td->time_frequency.QuadPart * PHYSICS_TICK)
    return;
  QueryPerformanceCounter(&td->time_start);

  Vector2 velocity = {0};
  if (IsKeyDown(KEY_LEFT))
  {
    velocity.x -= 1;
  }
  else if (IsKeyDown(KEY_RIGHT))
  {
    velocity.x += 1;
  }

  if (IsKeyDown(KEY_UP))
  {
    velocity.y -= 1;
  }
  else if (IsKeyDown(KEY_DOWN))
  {
    velocity.y += 1;
  }

  float wheel = GetMouseWheelMove();
  if (wheel != 0.f)
  {
    printf("wheel: %f\n", wheel);
    td->camera->zoom += (wheel * ((PHYSICS_TICK * 100.f) / 10));

    if (td->camera->zoom > 3.0f)
    {
      td->camera->zoom = 3.0f;
    }
    else if (td->camera->zoom < 0.01f)
    {
      td->camera->zoom = 0.01f;
    }

    td->request_update = true;
  }

  if (velocity.x != 0 || velocity.y != 0)
  {
    Vector2Normalize(velocity);
    velocity = Vector2Scale(velocity, 1.f / PHYSICS_TICK);

    td->camera->target.x += velocity.x;
    td->camera->target.y += velocity.y;

    td->player->box = get_center_box_from_camera(*(td->camera), td->player->box);
    td->request_update = true;
  }
}

int WINAPI update_thread(PVOID arg)
{
  ThreadData *td = arg;

  update_chunk_view(td->chunkView, td->chunkGroup, get_view_from_camera(*(td->camera)));
  swap_chunk_view(td->chunkView);

  while (td->is_active)
  {
    physics_update(td);

    if (!td->request_update || td->request_swap)
      continue;

    td->is_locked = true;
    update_chunk_view(td->chunkView, td->chunkGroup, get_view_from_camera(*(td->camera)));

    td->request_update = false;
    td->request_swap = true;

    td->is_locked = false;
  }

  return EXIT_SUCCESS;
}

/*
#ifdef _WIN32
DWORD WINAPI bridge_thread(LPVOID arg) {
#else
// pthread func
#endif

  BridgeThreadData *bridge = arg;

#ifndef _DEBUG
  Seed seed = { .seed = 0 };
#else
  SetRandomSeed((__int64)time(NULL));
  Seed seed = { .seed = GetRandomValue(0, 1000) };
#endif // _DEBUG


  char map_name[MAX_PATH];
  sprintf(map_name, "map_%lld", seed.seed);

  void *tmp = load_world(map_name, bridge->state->config->max_chunks / 2);
  if (!tmp)
  {
    tmp = new_world(bridge->state->config->max_chunks / 2,
                    bridge->state->config->max_chunks, seed, map_name);
  }

  if (!tmp) {
    return EXIT_FAILURE;
  }

   //void *tmp_wnc = bridge->world;
   //bridge->world = tmp;
   //sfree(tmp_wnc);


  Config *cfg = bridge->state->config;
  Camera2D *camera = bridge->camera;
  Player *player = bridge->player;
  World *world = bridge->world;

  ///
  unsigned int middle_index = world->len / 2;
  for (size_t i = 0; i < world->chunks[middle_index]->len; i++)
  {
          if (world->chunks[middle_index]->blocks[i].x == CHUNK_WIDTH / 2)
          {
                  player->position.x = (world->chunks[middle_index]->blocks[i].x
  * cfg->block_size) + (middle_index * CHUNK_WIDTH * cfg->block_size);
                  player->position.y = (world->chunks[middle_index]->blocks[i].y
  * cfg->block_size) - (cfg->block_size * 2); break;
          }
  }
  ///

  player->position.x =
      (world->current->blocks[0].x * cfg->block_size) +
      (world->current->position * CHUNK_WIDTH * cfg->block_size);
  player->position.y =
      (world->current->blocks[0].y * cfg->block_size) - (cfg->block_size * 2);

  bridge->camera->zoom = 1.f;
  bridge->camera->target.x =
      bridge->player->position.x - bridge->state->config->render_size / 2;
  bridge->camera->target.y =
      bridge->player->position.y - bridge->state->config->render_size / 2;

  bridge->player_rect = (Rectangle){player->position.x, player->position.y,
                                    cfg->block_size * 0.9, cfg->block_size * 2};
  Rectangle player_rect_next = bridge->player_rect;
  Rectangle block_rect = {
      .x = 0, .y = 0, .width = cfg->block_size, .height = cfg->block_size};

  unsigned int block_index = 0;

  const unsigned int max_blocks_distance = cfg->block_size * 2;

  const double dt = 1.0 / 120.0;

  const float damping = 0.7f;
  const float base_velocity = .4f * cfg->render_size;  // 200.f;
  const float min_velocity = base_velocity / 10.f;     // 20.f;

  double accumulator = 0.0;
  double frameTime = 0.0;

#ifdef _WIN32
  LARGE_INTEGER frequency, startTime, currentTime;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&startTime);
#else
// time
#endif
  Chunk *chunk = NULL;
  unsigned int position = 0;

  bridge->view_next.max.y = CHUNK_HEIGHT;

  while (bridge->active) {
    block_index = (camera->target.x / cfg->block_size);
    position = block_index / CHUNK_WIDTH;

    if (position != world->current->position) {
      update_world(world, position);
    }

    if (world->current != NULL) {
      bridge->view_current.min.x =
          floor((camera->target.x -
                 world->current->position * CHUNK_WIDTH * cfg->block_size) /
                cfg->block_size);
      bridge->view_current.max.x =
          min(bridge->view_current.min.x + cfg->max_render_block, CHUNK_WIDTH);

      bridge->view_current.max.y =
          ((int)(camera->target.y / cfg->block_size) % CHUNK_HEIGHT) +
          cfg->max_render_block;

      if (world->next != NULL) {
        bridge->view_next.max.x =
            (int)(bridge->view_current.min.x + cfg->max_render_block) %
            CHUNK_WIDTH;
        bridge->view_next.max.y = bridge->view_current.max.y;
      }
    }

    QueryPerformanceCounter(&currentTime);

    frameTime = (double)(currentTime.QuadPart - startTime.QuadPart) /
                frequency.QuadPart;
    accumulator += frameTime;
    startTime = currentTime;

    if (accumulator < dt) continue;
    accumulator -= dt;

    chunk = bridge->world->current;
    if (chunk == NULL) continue;

    position = bridge->world->current->position;

    if (bridge->world->next != NULL &&
        bridge->player->position.x >
            ((position + 1) * CHUNK_WIDTH * cfg->block_size)) {
      chunk = bridge->world->next;
      position++;
    } else if (bridge->world->next == NULL)
      continue;

    player->animation += dt * 4.f;
    update_player(player, base_velocity, dt);

    if (abs(player->velocity.x) > min_velocity || player->onground == 0) {
      player->state = ((int)player->animation % 2 == 0) ? P_WALK_1 : P_WALK_2;

      player->onground = 0;
      player_rect_next.x = player->position.x + player->velocity.x * dt;
      player_rect_next.y = player->position.y + player->velocity.y * dt;

      for (unsigned int i = 0; i < CHUNK_LEN; i++) {
        if (!chunk->blocks[i].type) continue;  // B_NONE

        block_rect.x = chunk->blocks[i].x * cfg->block_size +
                       position * CHUNK_WIDTH * cfg->block_size;
        block_rect.y = chunk->blocks[i].y * cfg->block_size;

        if (abs(block_rect.y - player_rect_next.y) > max_blocks_distance ||
            abs(block_rect.x - player_rect_next.x) > max_blocks_distance)
          continue;

        if (!CheckCollisionRecs(block_rect, player_rect_next)) continue;

        if (CheckCollisionRecs(
                block_rect, (Rectangle){.x = player_rect_next.x,
                                        .y = player->position.y,
                                        .width = player_rect_next.width,
                                        .height = player_rect_next.height})) {
          player->velocity.x = -player->velocity.x * dt;
        }

        if (CheckCollisionRecs(
                block_rect, (Rectangle){.x = player->position.x,
                                        .y = player_rect_next.y,
                                        .width = player_rect_next.width,
                                        .height = player_rect_next.height})) {
          player->velocity.y = 0;
          player->onground = 1;

          player->position.y =
              ((int)round(player->position.y / cfg->block_size)) *
              cfg->block_size;
          player_rect_next.y = player->position.y;
        }
      }

      player->velocity.x *= damping;
      player->velocity.y *= damping;

      if (player->velocity.y < 0 && abs(player->velocity.y) < min_velocity) {
        player->velocity.y = 0;
      }

      if (abs(player->velocity.x) < min_velocity) {
        player->velocity.x = 0;
      }

      player->position.x += player->velocity.x * dt;
      player->position.y += player->velocity.y * dt;
    } else {
      player->state = ((int)player->animation % 2 == 0) ? P_IDLE_1 : P_IDLE_2;
    }

    bridge->player_rect.x = player->position.x;
    bridge->player_rect.y = player->position.y;

    smooth_camera(camera->target.x, player->position.x - (cfg->render_size / 2),
                  base_velocity);
    smooth_camera(camera->target.y, player->position.y - (cfg->render_size / 2),
                  base_velocity);
  }

  return EXIT_SUCCESS;
}
*/