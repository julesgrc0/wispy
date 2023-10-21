#pragma once
#include "../stdafx.h"
#include "../core/state.h"

#define CHUNK_WIDTH  64  // 2^6
#define CHUNK_HEIGHT 256 // 2^8

typedef enum BlockTypes {
	B_NONE = 0,
	B_DIRT,
	B_GRASS,
	B_LEAVES,
	B_LOG,
	B_SAND,
	B_STONE
} BlockTypes;

#pragma pack(push,1)
typedef struct Block {
	unsigned int x : 6;   // 2^6 = 64
	unsigned int y : 8;  // 2^8 = 256

	unsigned int active : 1;
	BlockTypes type;
} Block;
typedef struct Chunk {
	Block* blocks;
	unsigned int len : 14; // 2^14 -> (2^6 * 2^8)
} Chunk;
#pragma pack(pop)

Chunk*	generate_chunk(Image image, unsigned int position);