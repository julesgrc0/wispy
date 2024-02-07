#pragma once
#include "../core/view.h"
#include "../stdafx.h"

#include "block.h"
#include "chunk.h"
#include "chunk_group.h"

typedef struct w_chunkview {
  w_chunk *target;
  w_chunk *next;
  w_renderblock *blocks;
  unsigned int len; // : 15; // 2^15 = 2^14 * 2

#ifdef _WIN32
  HANDLE mutex;
#else
  pthread_mutex_t mutex;
#endif // _WIN32

} w_chunkview;

w_chunkview *create_chunkview(w_chunk *current);
void destroy_chunkview(w_chunkview *chunk_view);

void update_renderblock_async(w_chunkview *chunk_view, w_renderblock *blocks,
                              size_t len);
bool update_chunkview(w_chunkview *chunk_view, w_chunkgroup *grp,
                      Rectangle view);
void filter_chunkview_blocks(w_chunk *chunk, Rectangle view,
                             w_renderblock *blocks, size_t *rendercount);