#include "game.h"

#define render_chunk(chunk, position)                                                                                                        \
	for (size_t i = 0; i < chunk->len; i++)                                                                                                  \
	{                                                                                                                                        \
		if (chunk->blocks[i].x >= box.max.x)                                                                                                 \
			break;                                                                                                                           \
		if (chunk->blocks[i].x < box.min.x)                                                                                                  \
			continue;                                                                                                                        \
		if (chunk->blocks[i].y >= box.max.y)                                                                                                 \
			continue;                                                                                                                        \
                                                                                                                                             \
		DrawTexturePro(                                                                                                                      \
			blocks[chunk->blocks[i].type - 1],                                                                                               \
			blockRect,                                                                                                                       \
			(Rectangle){                                                                                                                     \
				(chunk->blocks[i].x * cfg->block_size) + ((position)*CHUNK_WIDTH * cfg->block_size), (chunk->blocks[i].y * cfg->block_size), \
				cfg->block_size, cfg->block_size},                                                                                           \
			(Vector2){0}, 0, WHITE);                                                                                                         \
	}

void game_screen(State *state)
{
#ifndef _DEBUG
	HideCursor();
#endif // !_DEBUG

	Config *cfg = state->config;

	Texture blocks[3] = {
		get_texture_by_id(state, "blocks\\grass.png"),
		get_texture_by_id(state, "blocks\\dirt.png"),
		get_texture_by_id(state, "blocks\\stone.png")};

	Texture players[4] = {
		get_texture_by_id(state, "player\\player_idle1.png"),
		get_texture_by_id(state, "player\\player_idle2.png"),
		get_texture_by_id(state, "player\\player_walk1.png"),
		get_texture_by_id(state, "player\\player_walk2.png"),
	};

#ifdef _DEBUG
	World *world = generate_world(state->config->max_chunks, (Seed){.seed = 0});
#else
	SetRandomSeed(time(NULL));
	Seed seed = {
		.seed = GetRandomValue(0, 100)};

	char map_name[MAX_PATH];
	sprintf(map_name, "map_%lld.dat", seed.seed);

	World *world = load_world(map_name); // TODO: load, generate, export in a thread
	if (!world)
	{
		world = generate_world(state->config->max_chunks, seed);
		export_world(map_name, world);
	}
#endif // _DEBUG

	Camera2D *camera = malloc(sizeof(Camera2D));
	memset(camera, 0, sizeof(Camera2D));
	camera->zoom = 1.f;

	BoundingBox box = {0};
	Rectangle blockRect = {0, 0, blocks[0].width, blocks[0].height};
	short player_h = players[0].height;
	short player_w = players[0].width;

	int block_index = 0, index = 0, out_x = 0;

	Player *player = malloc(sizeof(Player));
	memset(player, 0, sizeof(Player));

	int mid_index = world->len / 2;
	Chunk *chunk = world->chunks[mid_index];
	for (size_t i = 0; i < chunk->len; i++)
	{
		if (chunk->blocks[i].x == CHUNK_WIDTH / 2)
		{
			player->position.x = (chunk->blocks[i].x * cfg->block_size) + (mid_index * CHUNK_WIDTH * cfg->block_size);
			player->position.y = (chunk->blocks[i].y * cfg->block_size) - (cfg->block_size * 2);
			break;
		}
	}

	ControllerThreadData *ctd = start_controller(state, camera, player);

	camera->target.x = player->position.x - state->config->render_size / 2;
	camera->target.y = player->position.y - state->config->render_size / 2;

	while (!WindowShouldClose())
	{

		BeginTextureMode(state->render);

		ClearBackground(BLACK);
		BeginMode2D(*camera);

		block_index = (camera->target.x / cfg->block_size);
		index = block_index / CHUNK_WIDTH;

		if (index < world->len)
		{
			ctd->chunk_current = world->chunks[index];
			ctd->position_current = index;

			box = (BoundingBox){
				.min = (Vector3){
					.x = block_index % CHUNK_WIDTH,
					.y = 0},
				.max = (Vector3){.x = block_index % CHUNK_WIDTH + cfg->max_render_block, .y = ((int)(camera->target.y / cfg->block_size) % CHUNK_HEIGHT) + cfg->max_render_block}};
			render_chunk(world->chunks[index], index)

				out_x = (CHUNK_WIDTH - (block_index % CHUNK_WIDTH)) - cfg->max_render_block;
			if (out_x < 0 && index + 1 < world->len)
			{
				box.min.x = 0;
				box.max.x = abs(out_x);

				render_chunk(world->chunks[index + 1], index + 1);

				ctd->chunk_next = world->chunks[index + 1];
				ctd->position_next = index + 1;
			}
			else
			{
				ctd->chunk_next = NULL;
				ctd->position_next = 0;
			}

			DrawTexturePro(players[player->state],
						   (Rectangle){0, 0, player_w * (player->direction ? -1 : 1), player_h},
						   (Rectangle){player->position.x, player->position.y, cfg->block_size, cfg->block_size * 2},
						   (Vector2){0}, 0, WHITE);
			DrawRectangleLines(
				player->position.x,
				player->position.y,
				cfg->block_size,
				cfg->block_size * 2,
				RED);
		}

		EndMode2D();
		EndTextureMode();

		BeginDrawing();
		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
		DrawFPS(0, 0);

		char info[500];
		sprintf(info, "Position: %f %f\nVel: %f %f\nOn ground: %d\nDelay: %f", player->position.x, player->position.y, player->velocity.x, player->velocity.y, player->onground, player->delay);

		DrawText(info, 0, 30, 20, WHITE);
		EndDrawing();
	}

	stop_controller(ctd);

	for (size_t i = 0; i < world->len; i++)
	{
		sfree(world->chunks[i]->blocks);
		sfree(world->chunks[i]);
	}
	sfree(world->chunks);
	sfree(world);

	sfree(player);
	sfree(camera);

#ifndef _DEBUG
	ShowCursor();
#endif // !_DEBUG
}
