#include "game.h"


static void render_chunk(State* state, int index, Rectangle blockDest, Rectangle blockSource, World* world)
{
	for (size_t i = 0; i < world->chunks[index]->len; i++)
	{
		blockDest.x = world->chunks[index]->blocks[i].x * BLOCK_SIZE + index * CHUNK_WIDTH * BLOCK_SIZE;
		blockDest.y = CHUNK_HEIGHT * BLOCK_SIZE - world->chunks[index]->blocks[i].y * BLOCK_SIZE;
		
		if (blockDest.x < (state->camera.target.x - BLOCK_SIZE) || blockDest.x > state->camera.target.x + RENDER_WIDTH || blockDest.y > state->camera.target.y + RENDER_HEIGHT || blockDest.y < (state->camera.target.y - BLOCK_SIZE)) continue;
		DrawTexturePro(state->textures[0], blockSource, blockDest, (Vector2) { 0 }, 0, WHITE);
	}
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
	char* map = "map.dat";
#else
	int seed = GetRandomValue(0, INT_MAX - 1);
#endif // _DEBUG
	
	world = generate_world(seed);
	/*
	TODO: Fix world export/load
	world = load_world(map);
	if (!world)
	{
		world = generate_world(seed);
		export_world(map, world);
	}
	*/

	
	
	float mid_size = (RENDER_WIDTH / 2);

	Rectangle blockSource = { 0, 0, blocks[0].width, blocks[0].height };
	Rectangle blockDest = { 0, 0, BLOCK_SIZE, BLOCK_SIZE };
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);
		
		BeginMode2D(state->camera);

		LARGE_INTEGER start, end, frequency;
		long long elapsed_time;

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);


		int index = state->camera.target.x / (float)RENDER_WIDTH;
		int rindex = round((state->camera.target.x + mid_size) / (float)RENDER_WIDTH);
		int findex = floor((state->camera.target.x - mid_size) / (float)RENDER_WIDTH);

		if (index < world->chunk_len) render_chunk(state, index, blockDest, blockSource, world);
		if (rindex != index && rindex < world->chunk_len) render_chunk(state, rindex, blockDest, blockSource, world);
		if (findex != index && findex < world->chunk_len) render_chunk(state, findex, blockDest, blockSource, world);
		
		QueryPerformanceCounter(&end);
		elapsed_time = (end.QuadPart - start.QuadPart) * 100000000 / frequency.QuadPart;
		printf("%lld ns\n", elapsed_time);

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
}
