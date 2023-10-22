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
		(chunk->blocks[i].x * block_size) + ((position) * CHUNK_WIDTH * block_size),(chunk->blocks[i].y * block_size), \
		block_size, block_size	},								\
		(Vector2){0}, 0,WHITE);									\
	}															

void game_screen(State* state)
{
	Texture blocks[3] = {
		get_texture_by_id(state,"blocks\\grass.png"),
		get_texture_by_id(state,"blocks\\dirt.png"),
		get_texture_by_id(state,"blocks\\stone.png")
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


	int block_index = 0, index = 0, out_x = 0;
	BoundingBox box = { 0 };
	Rectangle blockRect = { 0,0, blocks[0].width, blocks[0].height };

	float render_size = state->config->render_size;
	float fblock = render_size/(float)state->config->render_distance;
	
	int block_size = round(fblock);
	int max_render_block = round((render_size + fblock)/fblock);
	
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);

		BeginMode2D(state->camera);

		
		
		block_index = (state->camera.target.x/block_size);
		index = block_index/CHUNK_WIDTH;

		if (index < world->len)
		{
			 box = (BoundingBox)
			 {
				.min = (Vector3){
					.x = block_index % CHUNK_WIDTH,
					.y = 0
				},
				.max = (Vector3){
					.x = block_index % CHUNK_WIDTH + max_render_block,
					.y = ((int)(state->camera.target.y / block_size)%CHUNK_HEIGHT) + max_render_block
				}
			};
			 render_chunk(world->chunks[index], index)
			
			out_x = (CHUNK_WIDTH - (block_index % CHUNK_WIDTH)) - max_render_block;
			if (out_x < 0 && index + 1 < world->len) 
			{
				box.min.x = 0;
				box.max.x = abs(out_x);

				render_chunk(world->chunks[index + 1], index + 1);
			}
		}
		EndMode2D();

		float speed = GetFrameTime() * 300.f;
		if (IsKeyDown(KEY_UP))
		{
			state->camera.target.y -= speed;
		}
		else if (IsKeyDown(KEY_DOWN))
		{
			state->camera.target.y += speed;
		}

		if (IsKeyDown(KEY_LEFT))
		{
			state->camera.target.x -= speed;
		}
		else if (IsKeyDown(KEY_RIGHT))
		{
			state->camera.target.x += speed;
		}



		EndTextureMode();
		BeginDrawing();


		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);
//#ifdef _DEBUG
		DrawFPS(0, 0);
//#endif // _DEBUG

		EndDrawing();
	}

	for (size_t i = 0; i < world->len; i++)
	{
		sfree(world->chunks[i]->blocks);
		sfree(world->chunks[i]);
	}
	sfree(world->chunks);
	sfree(world);
}

