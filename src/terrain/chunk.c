#include "chunk.h"

w_chunkgroup *create_chunk_group(unsigned int position)
{
	w_chunkgroup *grp = malloc(sizeof(w_chunkgroup));
	grp->position = position - CHUNK_GROUP_MID_LEN;
	for (unsigned int x = 0; x < CHUNK_GROUP_LEN; x++)
	{
		grp->chunks[x] = create_chunk(grp->position + x, false);
	}
	return grp;
}

void next_chunk_group(w_chunkgroup *grp)
{
	grp->position += CHUNK_GROUP_MID_LEN;
	for (unsigned int x = 0; x < CHUNK_GROUP_MID_LEN; x++)
	{
		free(grp->chunks[x]);
		grp->chunks[x] = grp->chunks[x + CHUNK_GROUP_MID_LEN];
		grp->chunks[x + CHUNK_GROUP_MID_LEN] = create_chunk(grp->position + CHUNK_GROUP_MID_LEN + x, true);
	}
}

void prev_chunk_group(w_chunkgroup *grp)
{
	grp->position -= CHUNK_GROUP_MID_LEN;

	for (unsigned int x = 0; x < CHUNK_GROUP_MID_LEN; x++)
	{
		free(grp->chunks[x + CHUNK_GROUP_MID_LEN]);
		grp->chunks[x + CHUNK_GROUP_MID_LEN] = grp->chunks[x];
		grp->chunks[x] = create_chunk(grp->position + x, true);
	}
}

w_chunk *create_chunk(unsigned int position, bool thread)
{
	w_chunk *chunk = malloc(sizeof(w_chunk));
	chunk->position = position;
	chunk->handle = INVALID_HANDLE_VALUE;

	if (thread)
	{
		chunk->handle = CreateThread(NULL, 0, &create_chunk_thread, chunk, 0, 0);
	}
	else
	{
		create_chunk_thread(chunk);
	}

	return chunk;
}

#ifdef _WIN32
DWORD WINAPI create_chunk_thread(PVOID arg)
#else
int create_chunk_thread(void *arg)
#endif // _WIN32
{
	if (!arg)
		return;
	w_chunk *chunk = arg;

	Image topLayer = GenImagePerlinNoise(CHUNK_W, BLOCK_TOP_LAYER_H, chunk->position * CHUNK_W, 0, 6.f);
	Image mineral = GenImageCellular(CHUNK_W, CHUNK_MID_H - BLOCK_TOP_LAYER_H, 10);

	// Image caves = GenImageCellular(CHUNK_W, mineralHeight, 10);

	for (unsigned int x = 0; x < CHUNK_W; x++)
	{
		unsigned int level = 0;
		bool firstLayer = false;

		for (unsigned int y = 0; y < CHUNK_H; y++)
		{
			w_blocktype type = BLOCK_AIR;
			if (y >= CHUNK_MID_H)
			{
				if (y < CHUNK_MID_H + BLOCK_TOP_LAYER_H)
				{
					unsigned int value = GetImageColor(topLayer, x, (y - CHUNK_MID_H)).r;

					if (level == 0)
					{
						if (y > CHUNK_MID_H + BLOCK_TOP_LAYER_H / 3 || (value < 105))
						{
							type = BLOCK_GRASS;
							level++;
						}
					}
					else if (level < 10)
					{
						type = BLOCK_DIRT;
						if (value < 40)
						{
							type = BLOCK_STONE;
						}
						level++;
					}
					else
					{
						type = BLOCK_STONE;
						if (value < 60)
						{
							type = BLOCK_MINERAL;
						}
					}
				}
				else
				{
					unsigned int value = GetImageColor(mineral, x, y - (CHUNK_MID_H + BLOCK_TOP_LAYER_H)).r;
					type = BLOCK_MINERAL;
					if (!firstLayer)
					{
						type = BLOCK_STONE;
						if (value < 130 && value > 80)
						{
							firstLayer = true;
						}
					}
					else
					{
						if (value >= 70 && value <= 75)
						{
							type = BLOCK_MINERAL_IRON;
						}
						else if (value <= 20)
						{
							type = BLOCK_MINERAL_OR;
						}
					}
				}
			}

			chunk->blocks[y * CHUNK_W + x] = (w_block){.type = type};
		}
	}

	UnloadImage(topLayer);
	UnloadImage(mineral);

	chunk->handle = INVALID_HANDLE_VALUE;
}
