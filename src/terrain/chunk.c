#include "chunk.h"

Chunk* generate_chunk(Image image, unsigned int position)
{
    Chunk* chunk = malloc(sizeof(Chunk));
    chunk->position = position;
    chunk->len = 0;
    chunk->blocks = malloc(sizeof(Block) * (CHUNK_WIDTH * CHUNK_HEIGHT));
    

    for (unsigned int x = 0; x < CHUNK_WIDTH; x++)
    {
        Color color = GetImageColor(image, position * CHUNK_WIDTH + x, 0);
        bool first = true;

        for (unsigned int y = 0; y < CHUNK_HEIGHT; y++)
        {
            if (y < (color.r / 10))
            {
                Block block = { 0 };
                block.type = B_DIRT;

                block.x = x;
                block.y = y;
                block.interactable = first; // TODO: check if edge of next section

                chunk->blocks[chunk->len] = block;
                chunk->len++;

                first = !first;
            }
        }
    }
    chunk->blocks = realloc(chunk->blocks, sizeof(Block) * chunk->len);
    return chunk;
}
