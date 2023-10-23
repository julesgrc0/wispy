#include "chunk.h"

Chunk *generate_chunk(Image noise, unsigned int position)
{
    Chunk *chunk = malloc(sizeof(Chunk));
    if (!chunk)
        return NULL;

    chunk->blocks = malloc(sizeof(Block) * (CHUNK_WIDTH * CHUNK_HEIGHT));
    if (!chunk->blocks)
    {
        sfree(chunk);
        return NULL;
    }

    chunk->len = 0;

    for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
    {
        unsigned int lineHeight = GetImageColor(noise, position * CHUNK_WIDTH + x, 0).r / 10; // 5
        unsigned int level = 0;

        for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
        {
            if (y >= lineHeight)
            {
                BlockTypes type = B_GRASS;
                if (level > 0)
                {
                    unsigned int value = GetImageColor(noise, position * CHUNK_WIDTH + x, y).r;
                    if (level < 10 || value <= 80)
                    {
                        type = B_DIRT;
                    }
                    else
                    {
                        type = B_STONE;
                    }
                }

                level++;
                chunk->blocks[chunk->len] = (Block){
                    .x = x,
                    .y = y,
                    .type = type,
                    .active = 0};
                chunk->len++;
            }
        }
    }

    void *tmp = realloc(chunk->blocks, sizeof(Block) * chunk->len);
    if (!tmp)
    {
        sfree(chunk->blocks);
        sfree(chunk);
        return NULL;
    }

    chunk->blocks = tmp;
    return chunk;
}