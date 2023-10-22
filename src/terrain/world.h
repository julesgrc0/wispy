#pragma once
#include "../stdafx.h"
#include "../unpack/unpack.h"

#include "chunk.h"

#define BASE_CHUNK 100

typedef struct World {
	int seed;

	Chunk** chunks;
	unsigned int len;

} World;

World* generate_world(int seed);

World* load_world(char* map_name);
void export_world(char* map_name, World* w);