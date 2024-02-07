#include "chunk_view.h"

w_chunkview *create_chunkview(w_chunk *current) {
  w_chunkview *chunk_view = malloc(sizeof(w_chunkview));
  if (chunk_view == NULL) {
    LOG("failed to allocate memory for chunk view");
    return NULL;
  }
  chunk_view = memset(chunk_view, 0, sizeof(w_chunkview));

  chunk_view->blocks = malloc(chunk_view->len);
  if (chunk_view->blocks == NULL) {
    LOG("failed to allocate memory for chunk view blocks");
    free(chunk_view);
    return NULL;
  }
  chunk_view->target = current;

#ifdef _WIN32
  chunk_view->mutex = CreateMutexA(NULL, FALSE, NULL);
  if (chunk_view->mutex == INVALID_HANDLE_VALUE) {
    free(chunk_view->blocks);
    free(chunk_view);

    LOG("failed to create mutex for chunk view");
    return NULL;
  }
#else
  pthread_mutex_init(&chunk_view->mutex, NULL);
#endif // _WIN32

  LOG("creating chunk view");
  return chunk_view;
}

void destroy_chunkview(w_chunkview *chunk_view) {
  LOG("destroying chunk view");
#ifdef _WIN32
  CloseHandle(chunk_view->mutex);
#else
  pthread_mutex_destroy(&chunk_view->mutex);
#endif // _WIN32

  free(chunk_view->blocks);
  free(chunk_view);
}

void setlock_chunkview(w_chunkview *chunk_view, w_renderblock *blocks,
                       size_t len) {
#ifdef _WIN32
  WaitForSingleObject(chunk_view->mutex, INFINITE);
#else
  pthread_mutex_lock(&td->mutex);
#endif // _WIN32

  free(chunk_view->blocks);
  chunk_view->len = len;
  chunk_view->blocks = blocks;

#ifdef _WIN32
  ReleaseMutex(chunk_view->mutex);
#else
  pthread_mutex_unlock(&td->mutex);
#endif // _WIN32
}

bool update_chunkview(w_chunkview *chunk_view, w_chunkgroup *grp,
                      Rectangle view) {
  if (view.x < 0 || view.y < 0) {

    setlock_chunkview(chunk_view, NULL, 0);
    return true;
  }
  unsigned int viewx = floor(view.x / FULL_CHUNK_W);
  if (viewx != chunk_view->target->position) {
    int grpmove = need_chunkgroup_update(grp, viewx);
    if (grpmove > 0) {
      next_chunkgroup(grp);
    } else if (grpmove < 0) {
      prev_chunkgroup(grp);
    }

    chunk_view->target = get_chunkgroup_chunk(grp, viewx);
    if (chunk_view->target == NULL) {
      return false;
    }
  }

  unsigned int viewnextx =
      round((view.x + view.width) / FULL_CHUNK_W) * FULL_CHUNK_W;
  if (viewnextx != viewx) {
    chunk_view->next = get_chunkgroup_chunk(grp, viewnextx);
  } else {
    chunk_view->next = NULL;
  }

  size_t render_blocks = 0;
  size_t len = CHUNK_W * CHUNK_H * ((chunk_view->next == NULL) ? 1 : 2);
  w_renderblock *blocks = malloc(len * sizeof(w_renderblock));
  if (blocks == NULL) {
    return false;
  }
  memset(blocks, 0, len * sizeof(w_renderblock));

  Rectangle cube_box = {.x = 0, .y = 0, .width = CUBE_W, .height = CUBE_H};

  unsigned int start_x = 0; // abs(viewx - floor(view.x / CUBE_W));
  unsigned int end_x =
      CHUNK_W; // abs(viewx - round((view.x + view.width) / CUBE_W));
  unsigned int start_y = 0;     // floor(view.y / CUBE_H);
  unsigned int end_y = CHUNK_H; // round((view.y + view.height) / CUBE_H);

  for (unsigned int x = start_x; x < end_x; x++) {
    for (unsigned int y = start_y; y < end_y; y++) {

      unsigned int index = y * CHUNK_W + x;
      if (index >= CHUNK_W * CHUNK_H) {
        break;
      }
      if (chunk_view->target->blocks[index].type == BLOCK_AIR) {
        continue;
      }

      cube_box.x = (chunk_view->target->position * FULL_CHUNK_W) + (x * CUBE_W);
      cube_box.y = y * CUBE_H;
      if (CheckCollisionRecs(cube_box, view)) {

        blocks[render_blocks] =
            (w_renderblock){.dst = cube_box,
                            .src = CUBE_SRC_RECT,
                            // .light = 0,
                            .block = chunk_view->target->blocks[index]};
        render_blocks++;
      }
    }
  }
  if (render_blocks == 0) {
    free(blocks);
    setlock_chunkview(chunk_view, NULL, 0);
    return true;
  } else if (render_blocks != len) {
    w_renderblock *tmp = realloc(blocks, sizeof(w_renderblock) * render_blocks);
    if (tmp == NULL) {
      free(blocks);
      return false;
    }
    blocks = tmp;
  }

  setlock_chunkview(chunk_view, blocks, render_blocks);
  return true;
}
