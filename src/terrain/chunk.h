#pragma once
#include "../stdafx.h"
#include "block.h"
#include "terrain_data.h"

typedef struct w_chunk {
  w_block blocks[CHUNK_H * CHUNK_W]; // 2^14
  unsigned int position;

#if defined(WISPY_WINDOWS)
  HANDLE handle;
#elif defined(WISPY_LINUX) || defined(WISPY_ANDROID) || defined(WISPY_MACOS)
  pthread_t handle;
#endif

} w_chunk;

typedef enum w_chunkthread_state {
  CHUNKTHREAD_LOAD,
  CHUNKTHREAD_UNLOAD,
} w_chunkthread_state;

typedef struct w_chunkthread {
  w_chunk *chunk;
  w_chunkthread_state state;
} w_chunkthread;

w_chunk *load_chunk(unsigned int position);
w_chunk *load_chunk_async(unsigned int position);

void unload_chunk(w_chunk *chunk);
void unload_chunk_async(w_chunk *chunk);

bool load_chunk_blocks(w_chunk *chunk);
bool create_chunk_blocks(w_chunk* chunk);

#if defined(WISPY_WINDOWS)
DWORD WINAPI chunk_thread(PVOID arg);
#elif defined(WISPY_LINUX) || defined(WISPY_ANDROID) || defined(WISPY_MACOS)
void *chunk_thread(void *arg);
#endif