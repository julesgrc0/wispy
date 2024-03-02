#pragma once
#include "../stdafx.h"
#include "block.h"

typedef struct w_chunk {
  w_block blocks[CHUNK_H * CHUNK_W]; // 2^14
  unsigned int position;

#if defined(WISPY_WINDOWS)
  HANDLE handle;
#else
  pthread_t handle;
#endif

} w_chunk;

w_chunk *create_chunk(unsigned int position, bool thread);

#if defined(WISPY_WINDOWS)
DWORD WINAPI create_chunk_thread(PVOID arg);
#else
void *create_chunk_thread(void *arg);
#endif
