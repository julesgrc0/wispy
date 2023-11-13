#include "world.h"

Chunk** generate_chunks(unsigned int len, Seed seed)
{
    Chunk** chunks = malloc(sizeof(Chunk*));
    if (!chunks)
        return NULL;


    Image noise = GenImagePerlinNoise(CHUNK_WIDTH * len, CHUNK_HEIGHT, seed.offsetX, seed.offsetY, (32.f * len) / 10.f);

    for (unsigned int position = 0; position < len; position++)
    {
        chunks[position] = generate_chunk(noise, position);
    }

    UnloadImage(noise);
    return chunks;
}

void export_chunks(char* map_name, Chunk** chunks, unsigned int len)
{
    WorldThreadData* wtd = malloc(sizeof(WorldThreadData));
    if (!wtd)
        return;

    wtd->map_name = map_name;
    wtd->chunks = chunks;
    wtd->len = len;

    char path[MAX_PATH];
    path[0] = 0;
#ifdef _WIN32
    strcat(path, GetApplicationDirectory());
    strcat(path, "maps\\");
    if (!DirectoryExists(path))
        CreateDirectoryA(path, NULL);

    strcat(path, map_name);
    strcat(path, "\\");
    if (!DirectoryExists(path))
        CreateDirectoryA(path, NULL);

    CreateThread(NULL, NULL, &export_world_thread, wtd, 0, NULL);
#endif // _WIN32
}


World* new_world(unsigned int position, unsigned int len, Seed seed, char* map_name)
{
    Chunk** chunks = generate_chunks(len, seed);

    World* world = malloc(sizeof(World));
    world->map_name = map_name;

    char path[MAX_PATH];
    path[0] = 0;

    strcat(path, GetApplicationDirectory());
    strcat(path, "maps\\");
    strcat(path, map_name);
    strcat(path, "\\");

    FilePathList files = LoadDirectoryFiles(path);
    world->len = files.count;
    UnloadDirectoryFiles(files);

    if (position - 1 < len) world->prev = chunks[position - 1];
    if (position < len) world->current = chunks[position];
    if (position + 1 < len) world->next = chunks[position + 1];

    export_chunks(map_name, chunks, len);

    return world;
}

World* load_world(char* map_name, unsigned int base_position)
{
    char path[MAX_PATH];
    path[0] = 0;

    strcat(path, GetApplicationDirectory());
    strcat(path, "maps\\");
    strcat(path, map_name);
    strcat(path, "\\");

#ifdef _WIN32
    if (!DirectoryExists(path))
        return NULL;
#endif // _WIN32

    World* world = malloc(sizeof(World));
    if (!world)
        return NULL;

    world->map_name = map_name;
    
    FilePathList files = LoadDirectoryFiles(path);
    world->len = files.count;
    UnloadDirectoryFiles(files);

    if (base_position > world->len)
    {
        base_position = world->len / 2;
    }

    world->prev = load_chunk(map_name, base_position - 1);
    world->current = load_chunk(map_name, base_position);
    world->next = load_chunk(map_name, base_position + 1);

    return world;
}

void update_world(World* world, unsigned int new_position)
{
    if (world->current != NULL && world->current->position == new_position) return;

    if (world->prev != NULL && world->prev->position == new_position)
    {
        sfree(world->next);

        world->next = world->current;
        world->current = world->prev;

        if(world->current != NULL) 
            world->prev = load_chunk(world->map_name, world->current->position - 1);
        return;
    }

    if (world->next != NULL && world->next->position == new_position)
    {
        sfree(world->prev);

        world->prev = world->current;
        world->current = world->next;

        if (world->current != NULL) 
            world->next = load_chunk(world->map_name, world->current->position + 1);
        return;
    }

    sfree(world->prev);
    sfree(world->current);
    sfree(world->next);

    world->prev = load_chunk(world->map_name, new_position - 1);
    world->current = load_chunk(world->map_name, new_position);
    world->next = load_chunk(world->map_name, new_position + 1);
}

#ifdef _WIN32
DWORD WINAPI export_world_thread(LPVOID arg)
#else
int export_world_thread(void* arg)
#endif // _WIN32
{
    WorldThreadData* wtd = arg;
    for (unsigned int i = 0; i < wtd->len; i++)
    {
        export_chunk(wtd->map_name, wtd->chunks[i]);
        sfree(wtd->chunks[i])
    }
    sfree(wtd->chunks);
    sfree(wtd);
    return 0;
}
