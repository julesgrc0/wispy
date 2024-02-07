#pragma once
#include "../stdafx.h"
#include "block.h"

typedef struct w_chunk {
  w_block blocks[CHUNK_H * CHUNK_W]; // 2^14
  unsigned int position;
  HANDLE handle;
} w_chunk;

w_chunk *create_chunk(unsigned int position, bool thread);

#ifdef _WIN32
DWORD WINAPI create_chunk_thread(PVOID arg);
#else
int create_chunk_thread(void *arg);
#endif // _WIN32
