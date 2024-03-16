#include "chunk.h"

w_chunk *load_chunk(unsigned int position) {
  w_chunk *chunk = malloc(sizeof(w_chunk));
  if (chunk == NULL) {
    LOG("failed to allocate chunk");
    return NULL;
  }
  chunk->position = position;
  if (!load_chunk_blocks(chunk)) {
    LOG("failed to load chunk blocks");
    free(chunk);
    return NULL;
  }

  LOG("loading chunk: %u", position);
  return chunk;
}

w_chunk *load_chunk_async(unsigned int position) {
  w_chunk *chunk = malloc(sizeof(w_chunk));
  if (chunk == NULL) {
    LOG("failed to allocate chunk");
    return NULL;
  }
  chunk->position = position;

  w_chunkthread ct = {.chunk = chunk, .state = CHUNKTHREAD_LOAD};
#if defined(WISPY_WINDOWS)
  chunk->handle = CreateThread(NULL, 0, chunk_thread, &ct, 0, NULL);
  if (chunk->handle == NULL || chunk->handle == INVALID_HANDLE_VALUE)
#elif defined(WISPY_LINUX) || defined(WISPY_ANDROID)
  if (pthread_create(&chunk->handle, NULL, chunk_thread, &ct) != 0)
#endif
  {
    LOG("failed to create chunk thread (load)");
    free(chunk);
    return NULL;
  }

  LOG("loading chunk: %u", position);
  return chunk;
}

void unload_chunk(w_chunk *chunk) {
  if (chunk == NULL) {
    LOG("chunk (null) already unloaded");
    return;
  }

  if (!write_chunk_file(chunk->position, chunk->blocks)) {
    LOG("failed save chunk blocks %u", chunk->position);
  }
  LOG("unloading chunk: %u", chunk->position);
  free(chunk);
}

void unload_chunk_async(w_chunk *chunk) {
  if (chunk == NULL) {
    LOG("chunk (null) already unloaded");
    return;
  }

  w_chunkthread ct = {.chunk = chunk, .state = CHUNKTHREAD_UNLOAD};
#if defined(WISPY_WINDOWS)
  chunk->handle = CreateThread(NULL, 0, chunk_thread, &ct, 0, NULL);
  if (chunk->handle == NULL || chunk->handle == INVALID_HANDLE_VALUE)
#elif defined(WISPY_LINUX) || defined(WISPY_ANDROID)
  if (pthread_create(&chunk->handle, NULL, chunk_thread, &ct) != 0)
#endif
  {
    LOG("failed to create chunk thread (unload)");
    free(chunk);
    return;
  }
}

#if defined(WISPY_WINDOWS)
DWORD chunk_thread(PVOID arg)
#else
void *chunk_thread(void *arg)
#endif
{
  if (!arg) {
#if defined(WISPY_WINDOWS)
    return EXIT_FAILURE;
#else
    return NULL;
#endif
  }

  w_chunkthread *ct = arg;
  switch (ct->state) {
  case CHUNKTHREAD_LOAD:
    if (!load_chunk_blocks(ct->chunk)) {
      memset(ct->chunk->blocks, 0, CHUNK_W * CHUNK_H * sizeof(w_block));
      LOG("failed to load chunk blocks: %u", ct->chunk->position);
    }
    break;
  case CHUNKTHREAD_UNLOAD:
    unload_chunk(ct->chunk);
    break;
  }

#if defined(WISPY_WINDOWS)
  return EXIT_SUCCESS;
#else
  return NULL;
#endif
}

bool load_chunk_blocks(w_chunk *chunk) {
  char *chunk_path = get_terrain_path_chunk(chunk->position);
  if (chunk_path == NULL) {
    return false;
  }
  bool exists = FileExists(chunk_path);
  free(chunk_path);
  return  exists ? read_chunk_file(chunk->position, chunk->blocks)
                : create_chunk_blocks(chunk);
}

bool create_chunk_blocks(w_chunk *chunk) {
  Image base = GenImagePerlinNoise(CHUNK_W, BLOCK_TOP_LAYER_H,
                                   chunk->position * CHUNK_W, 0, 6.f);
  Image mineral =
      GenImageCellular(CHUNK_W, CHUNK_MID_H - BLOCK_TOP_LAYER_H, 10);

  for (unsigned int x = 0; x < CHUNK_W; x++) {
    unsigned int level = 0;
    bool firstLayer = false;

    for (unsigned int y = 0; y < CHUNK_H; y++) {
      w_blocktype type = BLOCK_AIR;
      if (y >= CHUNK_MID_H) {
        if (y < CHUNK_MID_H + BLOCK_TOP_LAYER_H) {
          unsigned int value = GetImageColor(base, x, (y - CHUNK_MID_H)).r;

          if (level == 0) {
            if (y > CHUNK_MID_H + BLOCK_TOP_LAYER_H / 3 || (value < 105)) {
              type = BLOCK_GRASS;
              level++;
            }
          } else if (level < 10) {
            type = BLOCK_DIRT;
            if (value < 40) {
              type = BLOCK_STONE;
            }
            level++;
          } else {
            type = BLOCK_STONE;
            if (value < 60) {
              type = BLOCK_MINERAL;
            }
          }
        } else {
          unsigned int value =
              GetImageColor(mineral, x, y - (CHUNK_MID_H + BLOCK_TOP_LAYER_H))
                  .r;
          type = BLOCK_MINERAL;
          if (!firstLayer) {
            type = BLOCK_STONE;
            if (value < 130 && value > 80) {
              firstLayer = true;
            }
          } else {
            if (value >= 70 && value <= 75) {
              type = BLOCK_MINERAL_IRON;
            } else if (value <= 20) {
              type = BLOCK_MINERAL_OR;
            }
          }
        }
      }

      chunk->blocks[y * CHUNK_W + x] =
          (w_block){.type = type, .is_background = false};
    }
  }

  UnloadImage(base);
  UnloadImage(mineral);

  return write_chunk_file(chunk->position, chunk->blocks);
}