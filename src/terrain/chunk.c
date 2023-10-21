#include "chunk.h"

Chunk* generate_chunk(Image noise, unsigned int position)
{

    Chunk* chunk = malloc(sizeof(Chunk));
    if (!chunk->blocks) return NULL;

 
    chunk->blocks = malloc(sizeof(Block) * (CHUNK_WIDTH * CHUNK_HEIGHT));
    chunk->len = 0;

    for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
    {
        Color color = GetImageColor(noise, position * CHUNK_WIDTH + x, 0);
        for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
        {
            if (y >= (color.r / 10))
            {
                chunk->blocks[chunk->len] = (Block){
                    .x = x,
                    .y = y,
                    .type = B_DIRT,
                    .active = 0
                };
                chunk->len++;
            }
        }
    }

    chunk->blocks = realloc(chunk->blocks, sizeof(Block) * chunk->len);
    return chunk;
}