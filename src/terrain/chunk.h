#pragma once
#include "../core/state.h"
#include "../stdafx.h"
#include "block.h"

#define CHUNK_LEN 64 * 256  // 2^14 -> (2^6 * 2^8)
#define CHUNK_WIDTH 64      // 2^6
#define CHUNK_HEIGHT 256    // 2^8

#pragma pack(push, 1)
typedef struct Chunk {
  Block blocks[CHUNK_LEN];
  unsigned int position;
} Chunk;
#pragma pack(pop)

Chunk* generate_chunk(Image image, unsigned int position);
Chunk* load_chunk(char* map_name, unsigned int position);
void export_chunk(char* map_name, Chunk* chunk);
