#include "chunk.h"

Rectangle get_center_box_from_camera(Camera2D camera, Rectangle box)
{
	return (Rectangle){
		.x = camera.target.x + ((RENDER_W * (1.f / camera.zoom) - box.width) / 2.f),
		.y = camera.target.y + ((RENDER_H * (1.f / camera.zoom) - box.height) / 2.f),
		.width = box.width,
		.height = box.height};
}

Rectangle get_view_from_camera(Camera2D camera)
{
	return (Rectangle){
		.x = camera.target.x, +((RENDER_W * (1.f / camera.zoom)) / 2.f), .y = camera.target.y, +((RENDER_H * (1.f / camera.zoom)) / 2.f), .width = RENDER_W * (1.f / camera.zoom), .height = RENDER_H * (1.f / camera.zoom)};
}

ChunkGroup *create_chunk_group(unsigned int position)
{
	ChunkGroup *grp = malloc(sizeof(ChunkGroup));
	grp->position = position - CHUNK_GROUP_MID_LEN;
	for (unsigned int x = 0; x < CHUNK_GROUP_LEN; x++)
	{
		grp->chunks[x] = create_chunk(grp->position + x, false);
	}

	return grp;
}
void next_chunk_group(ChunkGroup *grp)
{
	grp->position += CHUNK_GROUP_MID_LEN;

	/*unsigned*/ int x = 0;
#pragma omp parallel
	{
#pragma omp for
		for (x = 0; x < CHUNK_GROUP_MID_LEN; x++)
		{
			free(grp->chunks[x]);
			grp->chunks[x] = grp->chunks[x + CHUNK_GROUP_MID_LEN];
			grp->chunks[x + CHUNK_GROUP_MID_LEN] = create_chunk(grp->position + CHUNK_GROUP_MID_LEN + x, true);
		}
	}
}

void prev_chunk_group(ChunkGroup *grp)
{
	grp->position -= CHUNK_GROUP_MID_LEN;

	/*unsigned*/ int x = 0;
#pragma omp parallel
	{
#pragma omp for
		for (x = 0; x < CHUNK_GROUP_MID_LEN; x++)
		{
			free(grp->chunks[x + CHUNK_GROUP_MID_LEN]);
			grp->chunks[x + CHUNK_GROUP_MID_LEN] = grp->chunks[x];
			grp->chunks[x] = create_chunk(grp->position + x, true);
		}
	}
}

Chunk *create_chunk(unsigned int position, bool thread)
{
	Chunk *chunk = malloc(sizeof(Chunk));
	chunk->position = position;
	chunk->handle = INVALID_HANDLE_VALUE;

	if (thread)
	{
		chunk->handle = CreateThread(NULL, 0, create_chunk_thread, chunk, 0, 0);
	}
	else
	{
		create_chunk_thread(chunk);
	}

	return chunk;
}
DWORD WINAPI create_chunk_thread(PVOID arg)
{
	if (!arg)
		return;
	Chunk *chunk = arg;

	Image topLayer = GenImagePerlinNoise(CHUNK_W, BLOCK_TOP_LAYER_H, chunk->position * CHUNK_W, 0, 6.f);
	Image mineral = GenImageCellular(CHUNK_W, CHUNK_MID_H - BLOCK_TOP_LAYER_H, 10);

	// Image caves = GenImageCellular(CHUNK_W, mineralHeight, 10);

	int x = 0;
#pragma omp parallel
	{
#pragma omp for
		for (x = 0; x < CHUNK_W; x++)
		{
			unsigned int level = 0;
			bool firstLayer = false;

			for (unsigned int y = 0; y < CHUNK_H; y++)
			{
				BlockTypes type = BLOCK_AIR;
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

				chunk->blocks[y * CHUNK_W + x] = (Block){.type = type};
			}
		}
	}

	UnloadImage(topLayer);
	UnloadImage(mineral);

	chunk->handle = INVALID_HANDLE_VALUE;
}

ChunkView *create_chunk_view()
{
	ChunkView *chunkView = malloc(sizeof(ChunkView));
	chunkView = memset(chunkView, 0, sizeof(ChunkView));
	return chunkView;
}
void update_chunk_view(ChunkView *chunkView, ChunkGroup *grp, Rectangle view)
{
	Rectangle chunkBox =
		{
			.x = 0,
			.y = 0,
			.width = CHUNK_W * CUBE_W,
			.height = CHUNK_H * CUBE_H};

	int x = 0;

#pragma omp parallel
	{
#pragma omp for
		for (x = 0; x < CHUNK_GROUP_LEN; x++)
		{
			chunkBox.x = (grp->position * chunkBox.width) + (x * chunkBox.width);
			if (!CheckCollisionRecs(view, chunkBox))
				continue;

			chunkView->active = grp->chunks[x];
			if (x + 1 < CHUNK_GROUP_LEN)
			{
				chunkBox.x = (grp->position * chunkBox.width) + ((x + 1) * chunkBox.width);
				if (CheckCollisionRecs(view, chunkBox))
				{
					chunkView->next = grp->chunks[x + 1];
				}
				else
				{
					chunkView->next = NULL;
				}
			}
			else
			{
				chunkView->next = NULL;
			}

			break;
		}
	}

	if (chunkView->active != NULL)
	{
		Rectangle cubeBox = {
			.x = 0,
			.y = 0,
			.width = CUBE_W,
			.height = CUBE_H};

		chunkView->next_len = CHUNK_W * CHUNK_H * ((chunkView->next == NULL) ? 1 : 2);
		chunkView->next_blocks = malloc(chunkView->next_len * sizeof(RenderBlock));

		int index = 0, x = 0;
#pragma omp parallel
		{

#pragma omp for /*collapse(2)*/
			for (x = 0; x < CHUNK_W; x++)
			{
				for (unsigned int y = 0; y < CHUNK_H; y++)
				{
					cubeBox.x = chunkView->active->position * (chunkBox.width) + (x * CUBE_W);
					cubeBox.y = (y * CUBE_H);

					if (CheckCollisionRecs(cubeBox, view) && chunkView->active->blocks[y * CHUNK_W + x].type != BLOCK_AIR)
					{
						chunkView->next_blocks[index] = (RenderBlock){
							.dst = cubeBox,
							.src = CUBE_SRC_RECT,
							.light = 0,
							.block = chunkView->active->blocks[y * CHUNK_W + x]};
						index++;
					}

					if (chunkView->next == NULL)
						continue;

					cubeBox.x = chunkView->next->position * (chunkBox.width) + (x * CUBE_W);
					cubeBox.y = (y * CUBE_H);

					if (CheckCollisionRecs(cubeBox, view) && chunkView->next->blocks[y * CHUNK_W + x].type != BLOCK_AIR)
					{
						chunkView->next_blocks[index] = (RenderBlock){
							.dst = cubeBox,
							.src = CUBE_SRC_RECT,
							.light = 0,
							.block = chunkView->next->blocks[y * CHUNK_W + x]};
						index++;
					}
				}
			}
		}

		chunkView->next_len = index;
		chunkView->next_blocks = realloc(chunkView->next_blocks, sizeof(RenderBlock) * chunkView->next_len);
	}

	unsigned int camera_position = (int)round((view.x + view.width) / (CHUNK_W * CUBE_W));

	if (grp->position > 0 && camera_position <= (grp->position + CHUNK_GROUP_LOAD))
	{
		prev_chunk_group(grp);
	}
	else if (grp->position < MAXUINT && camera_position >= (grp->position + CHUNK_GROUP_LEN - CHUNK_GROUP_LOAD))
	{
		next_chunk_group(grp);
	}
}

void swap_chunk_view(ChunkView *chunkView)
{
	if (chunkView->next_blocks != NULL)
	{
		if (chunkView->blocks != NULL)
		{
			free(chunkView->blocks);
		}

		chunkView->blocks = chunkView->next_blocks;
		chunkView->len = chunkView->next_len;

		chunkView->next_blocks = NULL;
		chunkView->next_len = 0;
	}
}
