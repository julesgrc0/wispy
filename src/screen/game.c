#include "game.h"


#define render_chunk(chunk, position)							\
	for (size_t i = 0; i < chunk->len; i++)						\
	{															\
		if (chunk->blocks[i].x	>=	box.max.x)	break;			\
		if (chunk->blocks[i].x	<	box.min.x)	continue;		\
		if (chunk->blocks[i].y	>=	box.max.y)	continue;		\
																\
		DrawTexturePro(											\
		blocks[chunk->blocks[i].type - 1],						\
		blockRect,												\
		(Rectangle){											\
		(chunk->blocks[i].x * cfg->block_size) + ((position) * CHUNK_WIDTH * cfg->block_size),(chunk->blocks[i].y * cfg->block_size), \
		cfg->block_size, cfg->block_size	},					\
		(Vector2){0}, 0,WHITE);									\
	}															\
	DrawRectangleLines(position * CHUNK_WIDTH * cfg->block_size, 0, CHUNK_WIDTH * cfg->block_size, CHUNK_HEIGHT * cfg->block_size, RED);

void game_screen(State* state)
{
	Config* cfg = state->config;

	Texture blocks[3] = {
		get_texture_by_id(state,"blocks\\grass.png"),
		get_texture_by_id(state,"blocks\\dirt.png"),
		get_texture_by_id(state,"blocks\\stone.png")
	};

	Texture players[4] = {
		get_texture_by_id(state,"player\\player_idle1.png"),
		get_texture_by_id(state,"player\\player_idle2.png"),
		get_texture_by_id(state,"player\\player_walk1.png"),
		get_texture_by_id(state,"player\\player_walk2.png"),
	};

#ifdef _DEBUG
	World* world = generate_world(state->config->max_chunks, (Seed){ .seed = 0 });
#else
	SetRandomSeed(time(NULL));
	Seed seed = {
		.seed = GetRandomValue(0, 100) 
	};

	char map_name[MAX_PATH];
	sprintf(map_name, "map_%lld.dat", seed.seed);

	World* world = load_world(map_name); // TODO: load, generate, export in a thread
	if (!world)
	{
		world = generate_world(state->config->max_chunks, seed);
		export_world(map_name, world);
	}
#endif // _DEBUG

	Camera2D* camera = malloc(sizeof(Camera2D));
	memset(camera, 0, sizeof(Camera2D));
	camera->zoom = 1.f;


	BoundingBox box = { 0 };
	Rectangle blockRect = { 0,0, blocks[0].width, blocks[0].height };
	Rectangle playerRect = { 0,0, players[0].width, players[0].height };



	int block_index = 0, index = 0, out_x = 0;
	if (!world->player)
	{
		world->player = malloc(sizeof(Player));
		memset(world->player, 0, sizeof(Player));

		int mid_index = world->len / 2;
		Chunk* chunk = world->chunks[mid_index];
		for (size_t i = 0; i < chunk->len; i++)
		{
			if (chunk->blocks[i].x == CHUNK_WIDTH / 2)
			{
				world->player->position.x = chunk->blocks[i].x * cfg->block_size + (mid_index * CHUNK_WIDTH * cfg->block_size);
				world->player->position.y = chunk->blocks[i].y * cfg->block_size - cfg->block_size*2;
				break;
			}
		}

	}
	PlayerThreadData* pl_thread = start_player_thread(world->player, camera, state);

	while (!WindowShouldClose())
	{
		

		BeginTextureMode(state->render);
		ClearBackground(BLUE);
		BeginMode2D(*camera);

		block_index = (camera->target.x/cfg->block_size);
		index = block_index/CHUNK_WIDTH;

		if (index < world->len)
		{
			pl_thread->chunk = world->chunks[index];
			pl_thread->position = index;

			box = (BoundingBox)
			{
			   .min = (Vector3){
				   .x = block_index % CHUNK_WIDTH,
				   .y = 0
			   },
			   .max = (Vector3){
				   .x = block_index % CHUNK_WIDTH + cfg->max_render_block,
				   .y = ((int)(camera->target.y / cfg->block_size) % CHUNK_HEIGHT) + cfg->max_render_block
			   }
			};
			render_chunk(world->chunks[index], index)

				out_x = (CHUNK_WIDTH - (block_index % CHUNK_WIDTH)) - cfg->max_render_block;
			if (out_x < 0 && index + 1 < world->len)
			{
				box.min.x = 0;
				box.max.x = abs(out_x);

				render_chunk(world->chunks[index + 1], index + 1);
			}

			DrawTexturePro(players[world->player->state],
				playerRect,
				(Rectangle) { world->player->position.x, world->player->position.y, cfg->block_size, cfg->block_size*2 },
				(Vector2){ 0 }, 0, WHITE);
		}
		EndMode2D();
		EndTextureMode();

		BeginDrawing();
		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);
		DrawFPS(0, 0);
		EndDrawing();
	}
	stop_player_thread(pl_thread);
	
	for (size_t i = 0; i < world->len; i++)
	{
		sfree(world->chunks[i]->blocks);
		sfree(world->chunks[i]);
	}

	sfree(world->player);
	sfree(world->chunks);
	sfree(world);
	
	sfree(camera);
}

