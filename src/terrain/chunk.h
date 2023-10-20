#pragma once
#include "../stdafx.h"
#include "../core/state.h"
#include "block.h"

#define CHUNK_WIDTH  10 // 0-9 
#define CHUNK_HEIGHT 25 // 10 * 25 ~ sizeof(unsigned char) = 255
#define BLOCK_SIZE   RENDER_WIDTH/CHUNK_WIDTH

typedef struct Chunk {
	Block* blocks;
	size_t len;
} Chunk;

Chunk* generate_chunk(Image image, unsigned int position);