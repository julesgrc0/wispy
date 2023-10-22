#include "player.h"

PlayerThreadData* start_player_thread(Player* player, Camera2D* camera, State* state)
{
	PlayerThreadData* data = malloc(sizeof(PlayerThreadData));
	if (!data) return NULL;

	data->player = player;
	data->camera = camera;
	data->state = state;

	data->chunk = NULL;
	data->position = 0;

	data->active = 1;

	camera->target.x = player->position.x;
	camera->target.y = player->position.y;

#ifdef _WIN32
	data->handle = INVALID_HANDLE_VALUE;
	data->handle = CreateThread(NULL, NULL, &player_thread, data, 0, NULL);
#endif // _WIN32

	return data;
}

void stop_player_thread(PlayerThreadData* data)
{
	data->active = 0;

#ifdef _WIN32
	WaitForSingleObject(data->handle, INFINITE);
#endif // _WIN32

	sfree(data);
}

DWORD WINAPI player_thread(LPVOID arg)
{
	PlayerThreadData* data = (PlayerThreadData*)arg;
	Config* cfg = data->state->config;

	

	Rectangle block = { 0,0, cfg->block_size, cfg->block_size };
	Rectangle player = { 0,0, cfg->block_size, cfg->block_size * 2 };
	Vector2 next_position = { 0 };

	float dt = 0.f, next_dt;
	while (data->active)
	{
		next_dt = GetFrameTime();
		if (data->chunk == NULL || next_dt == dt) continue;
		dt = next_dt;

		float speed = GetFrameTime() * 300.f;
		next_position = data->player->position;

		if (IsKeyDown(KEY_SPACE))
		{
			next_position.y -= speed;
		}

		if (IsKeyDown(KEY_LEFT))
		{
			next_position.x -= speed;
		}
		else if (IsKeyDown(KEY_RIGHT))
		{
			next_position.x += speed;
		}


		// TODO: fix collision
		if (next_position.x != data->player->position.x || next_position.y != data->player->position.y)
		{
			player.x = next_position.y;
			player.y = next_position.x;

			size_t i = 0;
			for (; i < data->chunk->len; i++)
			{
				block.x = data->chunk->blocks[i].x * cfg->block_size + data->position * CHUNK_WIDTH * cfg->block_size;
				block.y = data->chunk->blocks[i].y * cfg->block_size;
				
				if (CheckCollisionRecs(block, player)) break;
			}

			if (i == data->chunk->len)
			{
				data->player->position = next_position;
			}
			else {
				printf("collision");
			}
			/*else
			{
				if (data->player->position.x != next_position.x)
				{
					data->player->position.x += next_position.x - block.x;
				}

				if (data->player->position.y != next_position.y)
				{
					data->player->position.y += next_position.y - block.x;
				}
			}*/
		}

		/*
		TODO: add gravity


		for (size_t i = 0; i < data->chunk->len; i++)
		{
			bx = data->chunk->blocks[i].x * cfg->block_size + data->position * CHUNK_WIDTH * cfg->block_size;
			px = data->player->position.x;
			if (bx != px) continue;

			by = data->chunk->blocks[i].y * cfg->block_size;
			if (by == py) break;

			if (py > by)
			{
				int diff = py - by;
				if (diff > speed)
				{
					data->player->position.y += diff;
				}else
				{
					data->player->position.y += speed;
				}
				break;
			}
		}
		*/

		data->camera->target.x = data->player->position.x - cfg->render_size/2;
		data->camera->target.y = data->player->position.y - cfg->render_size/2;
	}

	return 0;
}

/*
		

*/