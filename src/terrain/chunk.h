#pragma once
#include "../stdafx.h"
#include "../core/state.h"
#include "block.h"

#define CHUNK_WIDTH 64	 // 2^6
#define CHUNK_HEIGHT 256 // 2^8

typedef struct Chunk
{
	Block *blocks;
	unsigned int len : 14; // 2^14 -> (2^6 * 2^8)
} Chunk;

Chunk *generate_chunk(Image image, unsigned int position);