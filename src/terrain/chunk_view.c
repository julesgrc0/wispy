#include "chunk_view.h"

w_chunkview *create_chunk_view()
{
	w_chunkview *chunk_view = malloc(sizeof(w_chunkview));
	chunk_view = memset(chunk_view, 0, sizeof(w_chunkview));
	return chunk_view;
}

void update_chunk_view(w_chunkview *chunk_view, w_chunkgroup *grp, Rectangle view)
{
	Rectangle chunk_box =
	{
		.x = 0,
		.y = 0,
		.width = CHUNK_W * CUBE_W,
		.height = CHUNK_H * CUBE_H
	};

	for (unsigned int x = 0; x < CHUNK_GROUP_LEN; x++)
	{
		chunk_box.x = (grp->position * chunk_box.width) + (x * chunk_box.width);
		if (!CheckCollisionRecs(view, chunk_box))
			continue;

		chunk_view->active = grp->chunks[x];
		if (x + 1 < CHUNK_GROUP_LEN)
		{
			chunk_box.x = (grp->position * chunk_box.width) + ((x + 1) * chunk_box.width);
			if (CheckCollisionRecs(view, chunk_box))
			{
				chunk_view->next = grp->chunks[x + 1];
			}
			else
			{
				chunk_view->next = NULL;
			}
		}
		else
		{
			chunk_view->next = NULL;
		}

		break;
	}

	if (chunk_view->active != NULL)
	{
		Rectangle cube_box = {
			.x = 0,
			.y = 0,
			.width = CUBE_W,
			.height = CUBE_H};

		chunk_view->next_len = CHUNK_W * CHUNK_H * ((chunk_view->next == NULL) ? 1 : 2);
		chunk_view->next_blocks = malloc(chunk_view->next_len * sizeof(w_renderblock));

		unsigned int index = 0;

		for (unsigned int x = 0; x < CHUNK_W; x++)
		{
			for (unsigned int y = 0; y < CHUNK_H; y++)
			{
				cube_box.x = chunk_view->active->position * (chunk_box.width) + (x * CUBE_W);
				cube_box.y = (y * CUBE_H);

				if (CheckCollisionRecs(cube_box, view) && chunk_view->active->blocks[y * CHUNK_W + x].type != BLOCK_AIR)
				{
					chunk_view->next_blocks[index] = (w_renderblock){
						.dst = cube_box,
						.src = CUBE_SRC_RECT,
						.light = 0,
						.block = chunk_view->active->blocks[y * CHUNK_W + x]};
					index++;
				}

				if (chunk_view->next == NULL)
					continue;

				cube_box.x = chunk_view->next->position * (chunk_box.width) + (x * CUBE_W);
				cube_box.y = (y * CUBE_H);

				if (CheckCollisionRecs(cube_box, view) && chunk_view->next->blocks[y * CHUNK_W + x].type != BLOCK_AIR)
				{
					chunk_view->next_blocks[index] = (w_renderblock){
						.dst = cube_box,
						.src = CUBE_SRC_RECT,
						.light = 0,
						.block = chunk_view->next->blocks[y * CHUNK_W + x]};
					index++;
				}
			}
		}

		chunk_view->next_len = index;
		chunk_view->next_blocks = realloc(chunk_view->next_blocks, sizeof(w_renderblock) * chunk_view->next_len);
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

void swap_chunk_view(w_chunkview *chunk_view)
{
	if (chunk_view->next_blocks == NULL)
	{
		return;
	}

	if (chunk_view->blocks != NULL)
	{
		free(chunk_view->blocks);
	}

	chunk_view->blocks = chunk_view->next_blocks;
	chunk_view->len = chunk_view->next_len;

	chunk_view->next_blocks = NULL;
	chunk_view->next_len = 0;
}
