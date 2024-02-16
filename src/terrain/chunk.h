#pragma once
#include "../stdafx.h"
#include "block.h"

typedef struct w_chunk {
  w_block blocks[CHUNK_H * CHUNK_W]; // 2^14
  unsigned int position;

  #ifdef _WIN32
  HANDLE handle;
  #else
  pthread_t handle;
  #endif // _WIN32
  
} w_chunk;

w_chunk *create_chunk(unsigned int position, bool thread);

#ifdef _WIN32
DWORD WINAPI create_chunk_thread(PVOID arg);
#else
void* create_chunk_thread(void *arg);
#endif // _WIN32
