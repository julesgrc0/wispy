#pragma once
#include "../stdafx.h"
#include "../unpack/unpack.h"

#include "chunk.h"

typedef union Seed
{
    struct
    {
        int offsetX;
        int offsetY;
    };
    unsigned long long seed;
} Seed;

typedef struct World
{
    Seed seed;

    Chunk **chunks;
    unsigned int len;

} World;

World *generate_world(unsigned int len, Seed seed);

World *load_world(char *map_name);
void export_world(char *map_name, World *w);