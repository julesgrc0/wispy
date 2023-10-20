#pragma once
#include "../stdafx.h"
#include "../unpack/unpack.h"
#include "../entity/player.h"

#include "chunk.h"

#define BASE_CHUNK 100

typedef struct World {
	int seed;

	Chunk** chunks;
	unsigned int chunk_len;

	Player* player;
} World;

World* generate_world(int seed);
World* load_world(char* map);
void export_world(char* map, World* w);