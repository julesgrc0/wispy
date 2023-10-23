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

	float dt = 0.f, next_dt;
	float jump = 0.f;
	float velocity = 0.f;

	short stop_block_x = 0;
	while (data->active)
	{
		next_dt = GetFrameTime();
		if (data->chunk == NULL || next_dt == dt) continue;
		dt = next_dt;

		float speed = dt * 300.f;

		
		if (IsKeyPressed(KEY_SPACE) && jump != 0)
		{
			jump += speed * 2;
		}

		if (IsKeyDown(KEY_LEFT))
		{
			velocity = -speed;
		}
		else if (IsKeyDown(KEY_RIGHT))
		{
			velocity = speed;
		}


		if (velocity != 0 && !(velocity < 0 && stop_block_x < 0) && !(velocity > 0 && stop_block_x > 0))
		{
			player.x = data->player->position.x + velocity;
			player.y = (int)(data->player->position.y);

			size_t i = 0;
			for (; i < data->chunk->len; i++)
			{
				block.x = data->chunk->blocks[i].x * cfg->block_size + data->position * CHUNK_WIDTH * cfg->block_size;
				block.y = data->chunk->blocks[i].y * cfg->block_size;
				
				if (CheckCollisionRecs(block, player)) break;
			}

			
			if (i == data->chunk->len)
			{
				data->player->position.x += velocity;
				stop_block_x = 0;
			}else {
				if (velocity > 0)
				{
					data->player->position.x = block.x - cfg->block_size;
					stop_block_x = 1;
				}
				else if(velocity < 0)
				{
					data->player->position.x = block.x + cfg->block_size;
					stop_block_x = -1;
				}
			}

			velocity = 0.f;
		}

		/*
		if (jump > 0 || next_position != data->player->position.x)
		{
			int px = data->player->position.x;
			int bx = 0;
			for (size_t i = 0; i < data->chunk->len; i++)
			{
				bx = data->chunk->blocks[i].x * cfg->block_size + data->position * CHUNK_WIDTH * cfg->block_size;
				if (bx != px) continue;

				if (data->player->position.y > data->chunk->blocks[i].y)
				{
					float diff = data->player->position.y - data->chunk->blocks[i].y; // floor ?
					data->player->position.y += (diff > speed) ? diff : speed;
					break;
				}
			}
		}
		*/

		data->camera->target.x = data->player->position.x - cfg->render_size/2;
		data->camera->target.y = data->player->position.y - cfg->render_size/2;
	}

	return 0;
}