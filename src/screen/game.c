#include "game.h"


static rnd(World* world, unsigned int position, unsigned char min_x, unsigned char max_x)
{
	for (size_t i = 0; i < world->chunks[position]->len; i++)
	{
		if (world->chunks[position]->blocks[i].x >= max_x) break;

		if (world->chunks[position]->blocks[i].x < min_x) continue;
		if (world->chunks[position]->blocks[i].y >= 22) continue;
		

		
		DrawRectangleLines(
			(world->chunks[position]->blocks[i].x * BLOCK_SIZE) + (position * CHUNK_WIDTH * BLOCK_SIZE),
			(world->chunks[position]->blocks[i].y * BLOCK_SIZE),
			BLOCK_SIZE,
			BLOCK_SIZE,
			RED);
		
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
#else
	int seed = GetRandomValue(0, INT_MAX - 1);
#endif // _DEBUG

	char* map = "map.dat";
	world = load_world(map); // TODO: load, generate, export in a thread
	if (!world)
	{
		world = generate_world(seed);
		export_world(map, world);
	}

	int max_render_block_x = ((RENDER_SIZE + BLOCK_SIZE) / BLOCK_SIZE);
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);

		BeginMode2D(state->camera);

		
		
		int block_index = (state->camera.target.x / BLOCK_SIZE);
		int index = block_index / CHUNK_WIDTH;
		rnd(world, index, block_index % CHUNK_WIDTH, block_index % CHUNK_WIDTH + max_render_block_x);

		int out_x = (CHUNK_WIDTH - (block_index % CHUNK_WIDTH)) - max_render_block_x;
		if (out_x < 0) rnd(world, index + 1, 0, abs(out_x));
		
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

