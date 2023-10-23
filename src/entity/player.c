#include "player.h"

PlayerThreadData *start_player_thread(Player *player, Camera2D *camera, State *state)
{
	PlayerThreadData *data = malloc(sizeof(PlayerThreadData));
	if (!data)
		return NULL;

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

void stop_player_thread(PlayerThreadData *data)
{
	data->active = 0;

#ifdef _WIN32
	WaitForSingleObject(data->handle, INFINITE);
#endif // _WIN32

	sfree(data);
}

DWORD WINAPI player_thread(LPVOID arg)
{
	PlayerThreadData *data = (PlayerThreadData *)arg;
	Config *cfg = data->state->config;

	Rectangle block = {0, 0, cfg->block_size, cfg->block_size};
	Rectangle player = {0, 0, cfg->block_size, cfg->block_size * 2};

	float dt = 0.f, next_dt;
	float velocity = 0.f;
	float animation = 0.f;

	float target_y = 0.f;
	bool jump = false;
	bool is_grounded = false;

	float jump_delay = 0.f;

	while (data->active)
	{
		next_dt = GetFrameTime();
		if (next_dt == dt)
			continue;
		dt = next_dt;

		animation += dt * 5.f;

		if (IsKeyDown(KEY_RIGHT))
		{
			velocity = 200.f;
			data->player->direction = 0;
		}
		else if (IsKeyDown(KEY_LEFT))
		{
			velocity = -200.f;
			data->player->direction = 1;
		}

		if (IsKeyDown(KEY_SPACE) && target_y == 0 && !jump && is_grounded && jump_delay <= 0)
		{
			jump = true;
			target_y = data->player->position.y - player.height * 1.5;
		}
		else if (jump_delay > 0)
		{
			jump_delay -= dt;
		}

		if (velocity != 0 || jump || !is_grounded)
		{
			data->player->state = ((int)animation % 2 == 0) ? P_WALK_1 : P_WALK_2;

			Chunk *chunk = data->chunk_current;
			unsigned int position = data->position_current;

			if (data->player->position.x > ((data->position_current + 1) * CHUNK_WIDTH * cfg->block_size))
			{
				chunk = data->chunk_next;
				position = data->position_next;
			}

			player.x = data->player->position.x + velocity * dt;
			player.y = data->player->position.y;

			unsigned int px = round((player.x - position * CHUNK_WIDTH * cfg->block_size) / cfg->block_size);
			size_t i = 0;

			if (!jump)
			{
				target_y = 0;
			}

			for (; i < chunk->len; i++)
			{
				block.x = chunk->blocks[i].x * cfg->block_size + position * CHUNK_WIDTH * cfg->block_size;
				block.y = chunk->blocks[i].y * cfg->block_size;

				if (px == chunk->blocks[i].x && target_y == 0)
					target_y = block.y;

				if (CheckCollisionRecs(player, block))
				{
					player.x = (player.x > block.x) ? block.x + cfg->block_size : block.x - cfg->block_size;
				}
			}
			if (i == chunk->len)
				data->player->position.x = player.x;

			velocity = 0;
		}
		else
			data->player->state = ((int)animation % 2 == 0) ? P_IDLE_1 : P_IDLE_2;

		if (target_y > 0)
		{
			if (!jump)
			{
				data->player->position.y += dt * 200.f;
				if (target_y <= data->player->position.y + player.height)
				{
					data->player->position.y = target_y - player.height;
					target_y = 0;
					is_grounded = true;
				}
			}
			else
			{
				data->player->position.y -= dt * 400.f;
				if (target_y >= data->player->position.y)
				{
					data->player->position.y = target_y + player.height;
					target_y = 0;
					jump = false;
					is_grounded = false;
					jump_delay = 0.2f;
				}
			}
		}

		data->camera->target.x = data->player->position.x - cfg->render_size / 2;
		data->camera->target.y = data->player->position.y - cfg->render_size / 2;
	}
	return 0;
}
