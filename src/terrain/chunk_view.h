#pragma once
#include "../core/utils/camera.h"
#include "../stdafx.h"
#include "block.h"
#include "chunk.h"
#include "chunk_group.h"

#define DEFAULT_LIGHT_RADIUS 15 * CUBE_W

typedef struct w_chunkview {
  w_chunk *target;
  w_chunk *next;
  w_renderblock *blocks;
  size_t len; // : 15; // 2^15 = 2^14 * 2

#if defined(WISPY_WINDOWS)
  CRITICAL_SECTION csec;
#elif defined(WISPY_LINUX)
  pthread_mutex_t mutex;
#endif

} w_chunkview;

w_chunkview *create_chunkview(w_chunk *current);
void destroy_chunkview(w_chunkview *chunk_view);

void update_renderblock_async(w_chunkview *chunk_view, w_renderblock *blocks,
                              size_t len);
bool update_chunkview(w_chunkview *chunk_view, w_chunkgroup *grp,
                      w_camera *camera);
void filter_chunkview_blocks(w_chunk *chunk, Rectangle view,
                             w_renderblock *blocks, size_t *rendercount);

void update_chunkview_lighting(w_chunkview *chunk_view, Vector2 light,
                               float radius);

w_block *get_chunkview_block(w_chunkview *chunk_view, Vector2 position);
