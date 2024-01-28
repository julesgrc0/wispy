#include "bridge.h"

BridgeThreadData *start_thread(State *state) {
  BridgeThreadData *bridge = malloc(sizeof(BridgeThreadData));
  if (bridge == NULL) return NULL;
  memset(bridge, 0, sizeof(BridgeThreadData));

  bridge->active = 1;

  bridge->state = state;

  bridge->player = malloc(sizeof(Player));
  memset(bridge->player, 0, sizeof(Player));

  bridge->camera = malloc(sizeof(Camera2D));
  memset(bridge->camera, 0, sizeof(Camera2D));

  bridge->world = malloc(sizeof(World));
  memset(bridge->world, 0, sizeof(World));

#ifdef _WIN32
  bridge->handle = INVALID_HANDLE_VALUE;
  bridge->handle = CreateThread(NULL, 0, &bridge_thread, bridge, 0, NULL);
#endif  // _WIN32

  return bridge;
}

void stop_thread(BridgeThreadData *bridge) {
  bridge->active = 0;

#ifdef _WIN32
  if (bridge->handle != INVALID_HANDLE_VALUE)
    WaitForSingleObject(bridge->handle, INFINITE);
#endif  // _WIN32

  sfree(bridge->world->prev);
  sfree(bridge->world->current);
  sfree(bridge->world->next);
  sfree(bridge->world);

  sfree(bridge->player);
  sfree(bridge->camera);

  sfree(bridge);
}

#ifdef _WIN32
DWORD WINAPI bridge_thread(LPVOID arg) {
#else
// pthread func
#endif

  BridgeThreadData *bridge = arg;

  Seed seed = {.seed = 0};

  char map_name[MAX_PATH];
  sprintf(map_name, "map_%lld", seed.seed);

  void *tmp = load_world(map_name, bridge->state->config->max_chunks / 2);
  if (!tmp) {
    tmp = new_world(bridge->state->config->max_chunks / 2,
                    bridge->state->config->max_chunks, seed, map_name);
  }

  if (!tmp) {
    return EXIT_FAILURE;
  } else {
    void *tmp_wnc = bridge->world;
    bridge->world = tmp;
    sfree(tmp_wnc);
  }

  Config *cfg = bridge->state->config;
  Camera2D *camera = bridge->camera;
  Player *player = bridge->player;
  World *world = bridge->world;

  /*
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
  }*/

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
