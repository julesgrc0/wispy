#include "world.h"

World* generate_world(int seed)
{
    World* world = malloc(sizeof(World));
    world->player = NULL;
    
    world->chunk_len = BASE_CHUNK;
    world->chunks = malloc(sizeof(Chunk*) * world->chunk_len);

    world->seed = seed;

    Image image = GenImagePerlinNoise(CHUNK_WIDTH * world->chunk_len, 1, 0, seed, 32.f);//16.f);
    for (unsigned int i = 0; i < world->chunk_len; i++)
    {
        world->chunks[i] = generate_chunk(image, i);
    }

    // TODO: place the player at the center (BASE_CHUNK/2)

    UnloadImage(image);
    return world;
}

World* load_world(char* map)
{
    // TODO: decompress zlib

    FILE* in = fopen(map, "r");
    if (in == NULL) return NULL;

    World* w = (World*)malloc(sizeof(World));
    if (w == NULL) return NULL;

    fread(w, sizeof(World), 1, in);
    w->chunks = (Chunk**)malloc(sizeof(Chunk*) * w->chunk_len);

    for (size_t i = 0; i < w->chunk_len; i++)
    {
        w->chunks[i] = (Chunk*)malloc(sizeof(Chunk));
        fread(w->chunks[i], sizeof(Chunk), 1, in);

        w->chunks[i]->blocks = (Block*)malloc(sizeof(Block) * w->chunks[i]->len);
        fread(w->chunks[i]->blocks, sizeof(Block), w->chunks[i]->len, in);
    }

    fclose(in);
    return w;
}

void export_world(char* map, World* w)
{
    // TODO: compress zlib

    FILE* out = fopen(map, "w+");
    if (out == NULL) return;

    fwrite(w, sizeof(World), 1, out);

    for (size_t i = 0; i < w->chunk_len; i++)
    {
        fwrite(w->chunks[i], sizeof(Chunk), 1, out);
        fwrite(w->chunks[i]->blocks, sizeof(Block), w->chunks[i]->len, out);
    }

    fclose(out);
}


