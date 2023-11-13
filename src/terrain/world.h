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

typedef struct World {
    char* map_name;
    unsigned int len;

    Chunk* prev;
    Chunk* current;
    Chunk* next;

}World;

typedef struct WorldThreadData {
    char* map_name;

    Chunk** chunks;
    unsigned int len;
}WorldThreadData;

Chunk** generate_chunks(unsigned int len, Seed seed);
void export_chunks(char *map_name, Chunk** chunks, unsigned int len);

World* new_world(unsigned int position, unsigned int len, Seed seed, char* map_name);
World* load_world(char* map_name, unsigned int base_position);
void update_world(World* wnc, unsigned int new_position);

#ifdef _WIN32
DWORD WINAPI export_world_thread(LPVOID arg);
#else
int export_world_thread(void* arg);
#endif // _WIN32