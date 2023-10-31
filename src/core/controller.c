#include "controller.h"

ControllerThreadData* start_controller(State* state, Camera2D* camera, Player* player)
{
	ControllerThreadData* ctd = malloc(sizeof(ControllerThreadData));
	if (ctd == NULL) return NULL;

	ctd->active = 1;

	ctd->state = state;
	ctd->player = player;
	ctd->camera = camera;

	ctd->chunk_current = NULL;
	ctd->chunk_next = NULL;

	ctd->position_current = 0;
	ctd->position_next = 0;


	ctd->camera->target.x = ctd->player->position.x - ctd->state->config->render_size / 2;
	ctd->camera->target.y = ctd->player->position.y - ctd->state->config->render_size / 2;

#ifdef _WIN32
	ctd->handle = INVALID_HANDLE_VALUE;
	ctd->handle = CreateThread(NULL, NULL, &controller_thread, ctd, 0, NULL);
#endif // _WIN32

	return ctd;
}

void stop_controller(ControllerThreadData* ctd)
{
	ctd->active = 0;

#ifdef _WIN32
	if(ctd->handle != INVALID_HANDLE_VALUE) WaitForSingleObject(ctd->handle, INFINITE);
#endif // _WIN32

	sfree(ctd)
}

DWORD WINAPI controller_thread(LPVOID arg)
{
	ControllerThreadData* ctd = arg;
	Config* cfg = ctd->state->config;

	

	Camera2D* camera = ctd->camera;

	Player* player = ctd->player;
	player->velocity = (Vector2){ 0 };
	player->animation = 0.f;

	Rectangle playerRect = { .x = player->position.x, .y = player->position.y, .width = cfg->block_size, .height = cfg->block_size * 2 };

	Rectangle blockRect = { .x = 0, .y = 0, .width = cfg->block_size, .height = cfg->block_size };
	const unsigned int maxBlockDistance = cfg->block_size * 2;
	const float damping = 0.7f;
	const float minVelocity = 20.0f;

	double dt = 1.0 / (float)(cfg->max_fps == 0 ? 120 : cfg->max_fps);
	double accumulator = 0.0;
	double frameTime = 0.0;

	LARGE_INTEGER frequency, startTime, currentTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startTime);

	while (ctd->active)
	{
		QueryPerformanceCounter(&currentTime);

		frameTime = (double)(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
		accumulator += frameTime;
		startTime = currentTime;

		if (accumulator < dt) continue;
		accumulator -= dt;

		
		Chunk* chunk = ctd->chunk_current;
		unsigned int position = ctd->position_current;

		if (ctd->player->position.x > ((ctd->position_current + 1) * CHUNK_WIDTH * cfg->block_size))
		{
			chunk = ctd->chunk_next;
			position = ctd->position_next;
		}

		if (chunk == NULL) continue;

		player->animation += dt * 4.f;
		update_player(player, dt);

		if (abs(player->velocity.x) > minVelocity || player->onground == 0)
		{
			player->state = ((int)player->animation % 2 == 0) ? P_WALK_1 : P_WALK_2;

			player->onground = 0;
			playerRect.x = player->position.x + player->velocity.x * dt;
			playerRect.y = player->position.y + player->velocity.y * dt;

			for (size_t i = 0; i < chunk->len; i++)
			{
				blockRect.x = chunk->blocks[i].x * cfg->block_size + position * CHUNK_WIDTH * cfg->block_size;
				blockRect.y = chunk->blocks[i].y * cfg->block_size;

				if (abs(blockRect.y - playerRect.y) > maxBlockDistance || abs(blockRect.x - playerRect.x) > maxBlockDistance)
					continue;

				if (!CheckCollisionRecs(blockRect, playerRect))
					continue;


				if (CheckCollisionRecs(blockRect, (Rectangle){ 
					.x = playerRect.x,
					.y = player->position.y,
					.width = playerRect.width,
					.height = playerRect.height
					}))
				{
					player->velocity.x = -player->velocity.x * dt;
				}

				if (CheckCollisionRecs(blockRect, (Rectangle) {
					.x = player->position.x,
					.y = playerRect.y,
					.width = playerRect.width,
					.height = playerRect.height
					}))
				{

					player->velocity.y = 0;
					player->onground = 1;

					player->position.y = ((int)round(player->position.y / cfg->block_size)) * cfg->block_size;
				}


			}

			player->velocity.x *= damping;
			player->velocity.y *= damping;

			if (player->velocity.y < 0 && abs(player->velocity.y) < minVelocity)
			{
				player->velocity.y = 0;
			}

			if (abs(player->velocity.x) < minVelocity)
			{
				player->velocity.x = 0;
			}

			player->position.x += player->velocity.x * dt;
			player->position.y += player->velocity.y * dt;
		}
		else {
			player->state = ((int)player->animation % 2 == 0) ? P_IDLE_1 : P_IDLE_2;
		}
		

		smooth_camera(camera->target.x, player->position.x - (cfg->render_size / 2), 200.f);
		smooth_camera(camera->target.y, player->position.y - (cfg->render_size / 2), 200.f);
	}


	return 0;
}
