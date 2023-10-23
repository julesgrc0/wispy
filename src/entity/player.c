#include "player.h"

PlayerThreadData* start_player_thread(Player* player, Camera2D* camera, State* state)
{
	PlayerThreadData* data = malloc(sizeof(PlayerThreadData));
	if (!data) return NULL;

	data->player = player;
	data->camera = camera;
	data->state = state;

	data->chunk_current = NULL;
	data->position_current = 0;

	data->chunk_next = NULL;
	data->position_next = 0;


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
	float velocity = 0.f;
	float animation = 0.f;

	while (data->active)
	{
		next_dt = GetFrameTime();
		if (next_dt == dt) continue;
		dt = next_dt;

		animation += dt * 5.f;
		/*
		

		if (IsKeyPressed(KEY_SPACE) && target_y == 0)
		{
			target_y = data->player->position.y - player.height;
		}

		if (IsKeyDown(KEY_LEFT) && stop_block_x >= 0)
		{
			vel_x = -dt * 300.f;
			data->player->direction = 1;
		}
		else if (IsKeyDown(KEY_RIGHT) && stop_block_x <= 0)
		{
			vel_x = dt * 300.f;
			data->player->direction = 0;
		}



		if (target_y != 0)
		{
			if (target_y > data->player->position.y)
			{
				data->player->position.y += dt * 200.f;

				if (data->player->position.y > target_y)
				{
					data->player->position.y = target_y;
					target_y = 0;

				}
			}
			else if (target_y < data->player->position.y)
			{
				data->player->position.y -= dt * 150.f;

				if (data->player->position.y < target_y)
				{
					data->player->position.y = target_y;
					target_y = 0;

				}
			}

		}
		else if (data->player->position.y + player.height != target_block_y && target_block_y != 0)
		{
			target_y = target_block_y - player.height;
		}

		if (vel_x != 0 || target_y != 0)
		{
			


			player.x = data->player->position.x + vel_x;
			player.y = (int)(data->player->position.y);

			unsigned int index = round((player.x - data->position * CHUNK_WIDTH * cfg->block_size) / cfg->block_size);
			size_t i = 0;
			for (; i < data->chunk->len; i++)
			{
				if (index == data->chunk->blocks[i].x) break;
			}


			block.x = data->chunk->blocks[i].x * cfg->block_size + data->position * CHUNK_WIDTH * cfg->block_size;
			block.y = data->chunk->blocks[i].y * cfg->block_size;


			if (!CheckCollisionRecs(block, player))
			{
				data->player->position.x += vel_x;
				stop_block_x = 0;
				target_block_y = block.y;
			}
			else {
				target_block_y = block.y - cfg->block_size;
				stop_block_x = vel_x > 0 ? 1 : -1;
				data->player->position.x = vel_x > 0 ? block.x - cfg->block_size : block.x + cfg->block_size;
			}

			vel_x = 0.f;
		}
		
		*/

		if (IsKeyDown(KEY_RIGHT))
		{
			velocity = 300.f;
			data->player->direction = 0;
		}
		else if (IsKeyDown(KEY_LEFT))
		{
			velocity = -300.f;
			data->player->direction = 1;
		}

		if (velocity != 0)
		{
			data->player->state = ((int)animation % 2 == 0) ? P_WALK_1 : P_WALK_2;

			Chunk* chunk = data->chunk_current;
			unsigned int position = data->position_current;

			if (data->player->position.x > ((data->position_current + 1) * CHUNK_WIDTH * cfg->block_size))
			{
				chunk = data->chunk_next;
				position = data->position_next;
			}

			
			player.x = data->player->position.x + velocity * dt;
			player.y = data->player->position.y;

			unsigned int px = (player.x - position * CHUNK_WIDTH * cfg->block_size) / cfg->block_size;
			float target_y = 0.f;
			size_t i = 0;

			for (; i < chunk->len; i++)
			{
				block.x = chunk->blocks[i].x * cfg->block_size + position * CHUNK_WIDTH * cfg->block_size;
				block.y = chunk->blocks[i].y * cfg->block_size;

				if (px == chunk->blocks[i].x && target_y == 0) target_y = block.y;

				if (CheckCollisionRecs(player, block))
				{
					player.x = player.x > block.x ? block.x - cfg->block_size : block.x;
					if (target_y >= player.y - cfg->block_size) target_y = block.y;

					break;
				}
			}

			if (i == chunk->len) data->player->position.x = player.x;
			if (target_y != 0) data->player->position.y = target_y - player.height;
			
			velocity = 0;
		}
		else data->player->state = ((int)animation % 2 == 0) ? P_IDLE_1 : P_IDLE_2;

		data->camera->target.x = data->player->position.x - cfg->render_size / 2;
		data->camera->target.y = data->player->position.y - cfg->render_size / 2;

	}

	return 0;
}
