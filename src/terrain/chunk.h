#pragma once
#include "../stdafx.h"
#include "block.h"

#define CHUNK_MID_H 128
#define CHUNK_H 256 // 2^8
#define CHUNK_W 64	// 2^6

#define CHUNK_GROUP_LEN 10
#define CHUNK_GROUP_MID_LEN 5
#define CHUNK_GROUP_LOAD 2

typedef struct Chunk
{
	Block blocks[CHUNK_H * CHUNK_W]; // 2^14
	unsigned int position;
	HANDLE handle;
} Chunk;

typedef struct ChunkView
{
	Chunk *active;
	Chunk *next;

	RenderBlock *blocks;
	unsigned int len; // : 15; // 2^15 = 2^14 * 2

	RenderBlock *next_blocks;
	unsigned int next_len;

	bool lock;
} ChunkView;

typedef struct ChunkGroup
{
	Chunk *chunks[CHUNK_GROUP_LEN];
	unsigned int position;
} ChunkGroup;

Rectangle get_center_box_from_camera(Camera2D camera, Rectangle box);
Rectangle get_view_from_camera(Camera2D camera);

ChunkGroup *create_chunk_group(unsigned int position);
void next_chunk_group(ChunkGroup *);
void prev_chunk_group(ChunkGroup *);

Chunk *create_chunk(unsigned int position, bool thread);
DWORD WINAPI create_chunk_thread(PVOID arg);

ChunkView *create_chunk_view();
void update_chunk_view(ChunkView *chunkView, ChunkGroup *grp, Rectangle view);
void swap_chunk_view(ChunkView *chunkView);