#pragma once
#include "../stdafx.h"
#include "chunk.h"

typedef struct w_chunkgroup {
  w_chunk *chunks[CHUNK_GROUP_LEN];
  unsigned int position;
} w_chunkgroup;

w_chunkgroup *create_chunkgroup(unsigned int position);
void destroy_chunkgroup(w_chunkgroup *grp);

void next_chunkgroup(w_chunkgroup *grp);
void prev_chunkgroup(w_chunkgroup *grp);
int need_chunkgroup_update(w_chunkgroup *grp, unsigned int position);

w_chunk *get_chunkgroup_chunk(w_chunkgroup *grp, unsigned int position);
w_block *get_chunkgroup_block(w_chunkgroup *grp, Vector2 position);