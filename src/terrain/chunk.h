#pragma once
#include "../stdafx.h"
#include "block.h"

#define CHUNK_MID_H 128
#define CHUNK_H 256 // 2^8
#define CHUNK_W 64	// 2^6

#define CHUNK_GROUP_LEN 10
#define CHUNK_GROUP_MID_LEN 5
#define CHUNK_GROUP_LOAD 2

typedef struct w_chunk
{
	w_block blocks[CHUNK_H * CHUNK_W]; // 2^14
	unsigned int position;
	HANDLE handle;
} w_chunk;

typedef struct w_chunkgroup
{
	w_chunk *chunks[CHUNK_GROUP_LEN];
	unsigned int position;
} w_chunkgroup;

w_chunkgroup *create_chunk_group(unsigned int position);
void next_chunk_group(w_chunkgroup *);
void prev_chunk_group(w_chunkgroup *);

w_chunk *create_chunk(unsigned int position, bool thread);

#ifdef _WIN32
DWORD WINAPI create_chunk_thread(PVOID arg);
#else
int create_chunk_thread(void *arg);
#endif // _WIN32
