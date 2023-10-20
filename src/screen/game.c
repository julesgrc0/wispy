#include "game.h"

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

	// Move camera to State*
	Camera2D camera = {
		.offset = {0},
		.target = {RENDER_WIDTH / 2,RENDER_HEIGHT / 2},
		.zoom = .5f,
		.rotation = 0
	};

	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);
		
		BeginMode2D(camera);

		// TEST
		for (size_t k = 0; k < world->chunk_len; k++)
		{
			int baseX = world->chunks[k]->position * CHUNK_WIDTH * BLOCK_SIZE;
			for (size_t i = 0; i < world->chunks[k]->len; i++)
			{
				Block b = world->chunks[k]->blocks[i];
				DrawRectangleLines(b.x * BLOCK_SIZE + baseX, CHUNK_HEIGHT * BLOCK_SIZE - b.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, RED);
			}
		}

		float speed = GetFrameTime() * 300.f;
		if (IsKeyDown(KEY_UP))
		{
			camera.target.y -= speed;
		}
		else if (IsKeyDown(KEY_DOWN))
		{
			camera.target.y += speed;
		}

		if (IsKeyDown(KEY_LEFT))
		{
			camera.target.x -= speed;
		}
		else if (IsKeyDown(KEY_RIGHT))
		{
			camera.target.x += speed;
		}

		EndMode2D();

		EndTextureMode();
		BeginDrawing();


		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);
#ifdef _DEBUG
		DrawFPS(0, 0);
#endif // _DEBUG
		EndDrawing();
	}
}
