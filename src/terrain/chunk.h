#pragma once
#include "../stdafx.h"
#include "block.h"

#define CHUNK_SIZE 0

typedef struct Chunk {
	int position;

	Block* blocks;
	size_t len;
} Chunk;


