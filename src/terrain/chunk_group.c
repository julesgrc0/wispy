#include "chunk_group.h"

w_chunkgroup *create_chunkgroup(unsigned int position) {
  if (position >= (UINT_MAX - CHUNK_GROUP_LEN)) {
    LOG("max chunk group position reached");
    return NULL;
  }

  w_chunkgroup *grp = malloc(sizeof(w_chunkgroup));
  if (grp == NULL) {
    LOG("failed to allocate memory for chunk group");
    return NULL;
  }

  grp->position = position - CHUNK_GROUP_MID_LEN;
  for (unsigned int x = 0; x < CHUNK_GROUP_LEN; x++) {
    grp->chunks[x] = load_chunk(grp->position + x);
  }

  LOG("creating chunk group: %u", grp->position);
  return grp;
}

void destroy_chunkgroup(w_chunkgroup *grp) {
  if (grp == NULL) {
    LOG("chunk group (null) already destroyed");
    return;
  }

  LOG("destroying chunk group: %u", grp->position);
  for (unsigned int i = 0; i < CHUNK_GROUP_LEN; i++) {
#if defined(WISPY_WINDOWS)
    if (grp->chunks[i]->handle != INVALID_HANDLE_VALUE) {
      WaitForSingleObject(grp->chunks[i]->handle, INFINITE);
    }
#elif defined(WISPY_LINUX) || defined(WISPY_ANDROID) || defined(WISPY_MACOS)
    if (grp->chunks[i]->handle != 0) {
      pthread_join(grp->chunks[i]->handle, NULL);
    }
#endif

    unload_chunk(grp->chunks[i]);
  }
  free(grp);
}

void next_chunkgroup(w_chunkgroup *grp) {
  grp->position += CHUNK_GROUP_MID_LEN;
  LOG("loading next chunk group: %d", grp->position);
  for (unsigned int x = 0; x < CHUNK_GROUP_MID_LEN; x++) {
    unload_chunk_async(grp->chunks[x]);
    grp->chunks[x] = grp->chunks[x + CHUNK_GROUP_MID_LEN];
    grp->chunks[x + CHUNK_GROUP_MID_LEN] =
        load_chunk_async(grp->position + CHUNK_GROUP_MID_LEN + x);
  }
}

void prev_chunkgroup(w_chunkgroup *grp) {
  grp->position -= CHUNK_GROUP_MID_LEN;
  LOG("loading prev chunk group: %d", grp->position);
  for (unsigned int x = 0; x < CHUNK_GROUP_MID_LEN; x++) {
    unload_chunk_async(grp->chunks[x + CHUNK_GROUP_MID_LEN]);
    grp->chunks[x + CHUNK_GROUP_MID_LEN] = grp->chunks[x];
    grp->chunks[x] = load_chunk_async(grp->position + x);
  }
}

int need_chunkgroup_update(w_chunkgroup *grp, unsigned int position) {

  if (grp->position != 0 &&
      position <= grp->position + CHUNK_GROUP_UPDATE_DIST) {
    return -1;
  } else if (position != UINT_MAX && position > grp->position +
                                                    CHUNK_GROUP_LEN -
                                                    CHUNK_GROUP_UPDATE_DIST) {
    return 1;
  }
  return 0;
}

w_chunk *get_chunkgroup_chunk(w_chunkgroup *grp, unsigned int position) {
  if (position < grp->position ||
      position > (grp->position + CHUNK_GROUP_LEN - 1)) {
    return NULL;
  }
  return grp->chunks[position - grp->position];
}

w_block *get_chunkgroup_block(w_chunkgroup *grp, Vector2 position) {
  if (position.x < grp->position * CHUNK_W ||
      position.x >= (grp->position + CHUNK_GROUP_LEN) * CHUNK_W ||
      position.y < 0 || position.y >= CHUNK_H) {
    return NULL;
  }

  w_chunk *chunk = get_chunkgroup_chunk(grp, (unsigned int)(position.x / CHUNK_W));
  if (chunk == NULL) {
    return NULL;
  }

  return &chunk->blocks[(int)position.y * CHUNK_W +
                        ((int)(position.x - chunk->position * CHUNK_W) /
                         CHUNK_W)];
}