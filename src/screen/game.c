#include "game.h"


static rnd(World* world, Texture* blocks, unsigned int position, unsigned char min_x, unsigned char max_x)
{
	Chunk* chunk = world->chunks[position];
	for (size_t i = 0; i < chunk->len; i++)
	{
		if (chunk->blocks[i].x >= max_x) break;

		if (chunk->blocks[i].x < min_x) continue;
		if (chunk->blocks[i].y >= 22) continue;
		
		Texture block = { 0 };
		switch (chunk->blocks->type)
		{
		case B_STONE:
			block = blocks[5];
			break;
		case B_DIRT:
			block = blocks[0];
			break;
		case B_GRASS:
			block = blocks[1];
			break;
		}
		DrawTexturePro(block,
			(Rectangle) 
			{
				0, 0,
				block.width, block.height
			},

			(Rectangle)
			{
				(chunk->blocks[i].x * BLOCK_SIZE) + (position * CHUNK_WIDTH * BLOCK_SIZE),
				(chunk->blocks[i].y * BLOCK_SIZE),
				BLOCK_SIZE,
				BLOCK_SIZE 
			},
			(Vector2) { 0 },
			0,
			WHITE);
	}
	
	DrawRectangleLines(
		position * CHUNK_WIDTH * BLOCK_SIZE,
		0,
		CHUNK_WIDTH * BLOCK_SIZE,
		CHUNK_HEIGHT * BLOCK_SIZE,
		BLUE
	);
}

void game_screen(State* state)
{

Texture blocks[6] = {
		get_texture_by_id(state,"blocks\\dirt.png"),
		get_texture_by_id(state,"blocks\\grass.png"),
		get_texture_by_id(state,"blocks\\leaves.png"),
		get_texture_by_id(state,"blocks\\log.png"),
		get_texture_by_id(state,"blocks\\sand.png"),
		get_texture_by_id(state,"blocks\\stone.png")
	};

	World* world = NULL;

#ifdef _DEBUG
	int seed = 0;
	char* map_name = "map_dbg.dat";
#else
	int seed = GetRandomValue(0, INT_MAX - 1);
	char map_name[MAX_PATH];
	sprintf(map_name, "map_%d.dat", seed);
#endif // _DEBUG

	
	world = load_world(map_name); // TODO: load, generate, export in a thread
	if (!world)
	{
		world = generate_world(seed);
		export_world(map_name, world);
	}


	int max_render_block_x = ((RENDER_SIZE + BLOCK_SIZE) / BLOCK_SIZE);
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);

		BeginMode2D(state->camera);

		
		
		int block_index = (state->camera.target.x / BLOCK_SIZE);
		int index = block_index / CHUNK_WIDTH;
		if (index < world->len)
		{
			rnd(world,blocks, index, block_index % CHUNK_WIDTH, block_index % CHUNK_WIDTH + max_render_block_x);
			
			int out_x = (CHUNK_WIDTH - (block_index % CHUNK_WIDTH)) - max_render_block_x;
			if (out_x < 0 && index + 1 < world->len) 
			{
				rnd(world,blocks, index + 1, 0, abs(out_x));
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
#ifdef _DEBUG
		DrawFPS(0, 0);
#endif // _DEBUG

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

