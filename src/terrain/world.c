#include "world.h"

World* generate_world(unsigned int len, Seed seed)
{
    World* world = malloc(sizeof(World));
    if (!world) return NULL;

    world->player = NULL;
    world->seed = seed;
    world->len = len;
    world->chunks = malloc(sizeof(Chunk*) * world->len);
    
    if (!world->chunks)
    {
        sfree(world);
        return NULL;
    }
    
    Image noise = GenImagePerlinNoise(CHUNK_WIDTH * world->len, CHUNK_HEIGHT, seed.offsetX, seed.offsetY, (32.f * world->len)/10.f);

    for (unsigned int position = 0; position < world->len; position++)
    {
        world->chunks[position] = generate_chunk(noise, position);
    }

    UnloadImage(noise);
    return world;
}

World* load_world(char* map_name) {
    char path[MAX_PATH];
    path[0] = 0;

    strcat(path, GetApplicationDirectory());
    strcat(path, "maps\\");

#ifdef _WIN32
    if (!DirectoryExists(path)) CreateDirectoryA(path, NULL);
#endif // _WIN32

    strcat(path, map_name);
    if (!FileExists(path)) return NULL;

    size_t in_size = 0;
    char* in_data = LoadFileData(path, &in_size);
    if (!in_data) return NULL;

    size_t out_size = in_size;
    char* out_data = malloc(out_size);
    if (!out_data)
    {
        sfree(in_data);
        return NULL;
    }

    while (uncompress(out_data, &out_size, in_data, in_size) != Z_OK)
    {
        out_size += in_size;
        void* tmp = realloc(out_data, out_size);
        if (!tmp)
        {
            sfree(out_data);
            sfree(in_data);
        }
        out_data = tmp;
    }
    

    World* w = (World*)malloc(sizeof(World));
    
    size_t offset = 0;
    memcpy(&w->len, out_data + offset, sizeof(unsigned int));
    offset += sizeof(unsigned int);

    w->chunks = (Chunk**)malloc(sizeof(Chunk*) * w->len);

    memcpy(&w->seed, out_data + offset, sizeof(int));
    offset += sizeof(int);

    for (int i = 0; i < w->len; i++) {
        w->chunks[i] = (Chunk*)malloc(sizeof(Chunk));

        unsigned int len;
        memcpy(&len, out_data + offset, sizeof(unsigned int));
        w->chunks[i]->len = len;

        offset += sizeof(unsigned int);

        w->chunks[i]->blocks = (Block*)malloc(sizeof(Block) * len);
        for (int j = 0; j < w->chunks[i]->len; j++) {
            memcpy(&(w->chunks[i]->blocks[j]), out_data + offset, sizeof(Block));
            offset += sizeof(Block);
        }
    }

    sfree(in_data);
    sfree(out_data);

    return w;
}

void export_world(char* map_name, World* w) {
    size_t in_size = sizeof(unsigned int) + sizeof(int);
    for (int i = 0; i < w->len; i++) {
        in_size += sizeof(unsigned int) + (w->chunks[i]->len * sizeof(Block));
    }

    char* in_data = (char*)malloc(in_size);
    if (!in_data) return NULL;

    size_t offset = 0;
    memcpy(in_data + offset, &w->len, sizeof(unsigned int));
    offset += sizeof(unsigned int);

    memcpy(in_data + offset, &w->seed, sizeof(int));
    offset += sizeof(int);

    for (int i = 0; i < w->len; i++) {
        unsigned int len = w->chunks[i]->len;
        memcpy(in_data + offset, &len, sizeof(unsigned int));
        offset += sizeof(unsigned int);

        for (int j = 0; j < w->chunks[i]->len; j++) {
            memcpy(in_data + offset, &(w->chunks[i]->blocks[j]), sizeof(Block));
            offset += sizeof(Block);
        }
    }

    size_t out_size = in_size;
    char* out_data = malloc(out_size);

    if (compress(out_data, &out_size, in_data, in_size) != Z_OK)
    {
        sfree(in_data);
        sfree(out_data);
        return NULL;
    }


    char path[MAX_PATH];
    path[0] = 0;

    strcat(path, GetApplicationDirectory());
    strcat(path, "maps\\");

#ifdef _WIN32
    if (!DirectoryExists(path)) CreateDirectoryA(path, NULL);
#endif // _WIN32

    strcat(path, map_name);
    SaveFileData(path, out_data, out_size);
    
    sfree(in_data);
    sfree(out_data);
}

