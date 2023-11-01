#include "bridge.h"

BridgeThreadData *start_thread(State *state)
{
	BridgeThreadData *bridge = malloc(sizeof(BridgeThreadData));
	if (bridge == NULL)
		return NULL;
	memset(bridge, 0, sizeof(BridgeThreadData));

	bridge->active = 1;

	bridge->state = state;

	bridge->player = malloc(sizeof(Player));
	memset(bridge->player, 0, sizeof(Player));

	bridge->camera = malloc(sizeof(Camera2D));
	memset(bridge->camera, 0, sizeof(Camera2D));

#ifdef _WIN32
	bridge->handle = INVALID_HANDLE_VALUE;
	bridge->handle = CreateThread(NULL, NULL, &bridge_thread, bridge, 0, NULL);
#endif // _WIN32

	return bridge;
}

void stop_thread(BridgeThreadData *bridge)
{
	bridge->active = 0;

#ifdef _WIN32
	if (bridge->handle != INVALID_HANDLE_VALUE)
		WaitForSingleObject(bridge->handle, INFINITE);
#endif // _WIN32

	for (size_t i = 0; i < bridge->world->len; i++)
	{
		sfree(bridge->world->chunks[i]->blocks);
		sfree(bridge->world->chunks[i]);
	}
	sfree(bridge->world->chunks);
	sfree(bridge->world);

	sfree(bridge->player);
	sfree(bridge->camera);

	sfree(bridge);
}

DWORD WINAPI bridge_thread(LPVOID arg)
{
	BridgeThreadData *bridge = arg;


	/*
	TODO: 
	- only load the chunks near the player in memory, not all the chunks
	- export the world in another thread
	- display a loading world animation (load in a thread)
	*/
#ifdef _DEBUG
	bridge->world = generate_world(bridge->state->config->max_chunks, (Seed){.seed = 0});
#else
	SetRandomSeed(time(NULL));
	Seed seed = {
		.seed = GetRandomValue(0, 100)};

	char map_name[MAX_PATH];
	sprintf(map_name, "map_%lld.dat", seed.seed);

	bridge->world = load_world(map_name);
	if (!bridge->world)
	{
		bridge->world = generate_world(bridge->state->config->max_chunks, seed);
		export_world(map_name, bridge->world);
	}
#endif // _DEBUG

	Config *cfg = bridge->state->config;
	Camera2D *camera = bridge->camera;
	Player *player = bridge->player;
	World *world = bridge->world;

	unsigned int middle_index = world->len / 2;
	for (size_t i = 0; i < world->chunks[middle_index]->len; i++)
	{
		if (world->chunks[middle_index]->blocks[i].x == CHUNK_WIDTH / 2)
		{
			player->position.x = (world->chunks[middle_index]->blocks[i].x * cfg->block_size) + (middle_index * CHUNK_WIDTH * cfg->block_size);
			player->position.y = (world->chunks[middle_index]->blocks[i].y * cfg->block_size) - (cfg->block_size * 2);
			break;
		}
	}

	bridge->camera->zoom = 1.f;
	bridge->camera->target.x = bridge->player->position.x - bridge->state->config->render_size / 2;
	bridge->camera->target.y = bridge->player->position.y - bridge->state->config->render_size / 2;

	bridge->player_rect = (Rectangle){
		player->position.x,
		player->position.y,
		cfg->block_size,
		cfg->block_size * 2};
	Rectangle player_rect_next = bridge->player_rect;
	Rectangle block_rect = {.x = 0, .y = 0, .width = cfg->block_size, .height = cfg->block_size};

	unsigned int block_index = 0;

	const unsigned int maxBlockDistance = cfg->block_size * 2;
	const float damping = 0.7f;
	const float minVelocity = 20.0f;

	const double dt = 1.0/120.0;
	const float baseVelocity = 0.4f * cfg->render_size;

	double accumulator = 0.0;
	double frameTime = 0.0;

	LARGE_INTEGER frequency, startTime, currentTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startTime);

	while (bridge->active)
	{
		block_index = (camera->target.x / cfg->block_size);
		bridge->position_current = block_index / CHUNK_WIDTH;

		if (bridge->position_current < world->len)
		{
			bridge->chunk_current = world->chunks[bridge->position_current];

			bridge->camera_view_current.min.x = block_index % CHUNK_WIDTH;
			bridge->camera_view_current.max.x = block_index % CHUNK_WIDTH + cfg->max_render_block;
			bridge->camera_view_current.max.y = ((int)(camera->target.y / cfg->block_size) % CHUNK_HEIGHT) + cfg->max_render_block;

			bridge->position_next = bridge->position_current + 1;
			int out_x = (CHUNK_WIDTH - (block_index % CHUNK_WIDTH)) - cfg->max_render_block;
			if (bridge->position_next < world->len && out_x < 0)
			{
				bridge->chunk_next = world->chunks[bridge->position_next];

				bridge->camera_view_next.max.x = abs(out_x);
				bridge->camera_view_next.max.y = bridge->camera_view_current.max.y;
			}
			else if (bridge->chunk_next != NULL)
			{
				bridge->chunk_next = NULL;
			}
		}

		QueryPerformanceCounter(&currentTime);

		frameTime = (double)(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
		accumulator += frameTime;
		startTime = currentTime;

		if (accumulator < dt)
			continue;
		accumulator -= dt;

		Chunk *chunk = bridge->chunk_current;
		unsigned int position = bridge->position_current;

		if (bridge->player->position.x > ((bridge->position_current + 1) * CHUNK_WIDTH * cfg->block_size))
		{
			chunk = bridge->chunk_next;
			position = bridge->position_next;
		}

		if (chunk == NULL)
			continue;

		player->animation += dt * 4.f;
		update_player(player, baseVelocity, dt);

		if (abs(player->velocity.x) > minVelocity || player->onground == 0)
		{
			player->state = ((int)player->animation % 2 == 0) ? P_WALK_1 : P_WALK_2;

			player->onground = 0;
			player_rect_next.x = player->position.x + player->velocity.x * dt;
			player_rect_next.y = player->position.y + player->velocity.y * dt;

			for (size_t i = 0; i < chunk->len; i++)
			{
				block_rect.x = chunk->blocks[i].x * cfg->block_size + position * CHUNK_WIDTH * cfg->block_size;
				block_rect.y = chunk->blocks[i].y * cfg->block_size;

				if (abs(block_rect.y - player_rect_next.y) > maxBlockDistance || abs(block_rect.x - player_rect_next.x) > maxBlockDistance)
					continue;

				if (!CheckCollisionRecs(block_rect, player_rect_next))
					continue;

				if (CheckCollisionRecs(block_rect, (Rectangle){
													   .x = player_rect_next.x,
													   .y = player->position.y,
													   .width = player_rect_next.width,
													   .height = player_rect_next.height}))
				{
					player->velocity.x = -player->velocity.x * dt;
				}

				if (CheckCollisionRecs(block_rect, (Rectangle){
													   .x = player->position.x,
													   .y = player_rect_next.y,
													   .width = player_rect_next.width,
													   .height = player_rect_next.height}))
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
		else
		{
			player->state = ((int)player->animation % 2 == 0) ? P_IDLE_1 : P_IDLE_2;
		}

		bridge->player_rect.x = player->position.x;
		bridge->player_rect.y = player->position.y;

		smooth_camera(camera->target.x, player->position.x - (cfg->render_size / 2), baseVelocity);
		smooth_camera(camera->target.y, player->position.y - (cfg->render_size / 2), baseVelocity);
	}

	return 0;
}
