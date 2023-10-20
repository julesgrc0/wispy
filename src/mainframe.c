#include "mainframe.h"

State* init_mainframe()
{
	State* state = malloc(sizeof(State));
	if (state == NULL) return NULL;

	state->loading = LS_DISPLAY;

	state->len = 0;
	state->textures = NULL;
	state->textures_id = NULL;
	state->font = (Font){ 0 };
	state->render = (RenderTexture){ 0 };
	state->src_rnd = (Rectangle){ 0 };
	state->dest_rnd = (Rectangle){ 0 };

	// TODO:
	// - check if terrain map exists
	// - load user config

#ifdef _DEBUG
	SetTraceLogLevel(LOG_ALL);
	SetExitKey(KEY_ESCAPE);
#else
	SetExitKey(0);
	SetTraceLogLevel(LOG_NONE);
#endif // _DEBUG


	//SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE);
	SetTargetFPS(0);

	InitWindow(500, 500, "Wispy");

	return state;
}

void destroy_mainframe(State* state)
{
	if (state == NULL) return;
	

	for (size_t i = 0; i < state->len; i++)
	{
		sfree(state->textures_id[i]);
		UnloadTexture(state->textures[i]);
	}

	sfree(state->textures);
	sfree(state->textures_id);

	UnloadFont(state->font);
	UnloadRenderTexture(state->render);
	CloseWindow();
	
	free(state);
}

void loop_mainframe(State* state)
{
	loading_screen(state);
	if (state->loading != LS_OK) return;

	// TODO: menu screen (maps, config, etc...)

	game_screen(state);
}

Texture get_texture_from_id(State* state, char* id)
{
	for (size_t i = 0; i < state->len; i++)
	{
		if (strcmp(state->textures_id[i], id) == 0)
		{
			return state->textures[i];
		}
	}

	return (Texture){ 0 };
}

void load_assets(State* state)
{

	state->render = LoadRenderTexture(960, 540);
	state->src_rnd = (Rectangle){ 0.0f, 0.0f, (float)state->render.texture.width, -(float)state->render.texture.height };
	state->dest_rnd = (Rectangle){ 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
	
	size_t len;
	AssetItem* items = unpack_assets(state->hInstance, &len);
	if (!items)
	{
		state->loading = LS_FAILED;
		return;
	}

	state->textures = malloc(sizeof(Texture) * len);
	state->textures_id = malloc(sizeof(char*) * len);
	
	size_t textures_index = 0;
	for (size_t i = 0; i < len; i++)
	{
		const char* ext = strrchr(items[i].name, '.');

		if (strcmp(ext, ".png") == 0)
		{
			Image image = LoadImageFromMemory(".png", items[i].buffer, items[i].size);
			state->textures[textures_index] = LoadTextureFromImage(image);
			UnloadImage(image);

			state->textures_id[textures_index] = items[i].name;
			textures_index++;
		}
		else if (strcmp(ext, ".ttf") == 0)
		{
			const int fontSize = 72;
			const char fontChars[73] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()?:+-=*\"'";
			state->font = LoadFontFromMemory(".ttf", items[i].buffer, items[i].size, fontSize, fontChars, 73);
			
			sfree(items[i].name);
		}
		else 
		{
			sfree(items[i].name);
			sfree(items[i].buffer);
			continue;
		}
		
		sfree(items[i].buffer);
	}

	state->len = textures_index;
	state->textures = realloc(state->textures, sizeof(Texture) * state->len);
	state->textures_id = realloc(state->textures_id, sizeof(char*) * state->len);

	sfree(items);

	state->loading = LS_OK;
}

void load_config(State* state)
{

}

void loading_screen(State* state)
{
	bool loaded = false;

	int text_size = 50;

	const char* loading_text = "Loading...";
	const char* error_text = "Failed to load resources !";

	Vector2 loading_pos = {
		.x = (GetScreenWidth() - MeasureText(loading_text, text_size)) / 2.f,
		.y = (GetScreenHeight() - text_size) / 2.f
	};

	Vector2 error_pos = {
		.x = (GetScreenWidth() - MeasureText(error_text, text_size)) / 2.f,
		.y = (GetScreenHeight() - text_size) / 2.f
	};

	while (!loaded && !WindowShouldClose())
	{
		switch (state->loading)
		{
		case LS_DISPLAY:
		{
			BeginDrawing();
			ClearBackground(BLACK);
			DrawText(loading_text, loading_pos.x, loading_pos.y, text_size,  WHITE);
			EndDrawing();
			state->loading = LS_LOAD;
		}
		break;
		case LS_LOAD:
			load_assets(state);
			break;
		case LS_FAILED:
		{
			BeginDrawing();
			ClearBackground(BLACK);
			DrawText(error_text, error_pos.x, error_pos.y, text_size, RED);
			EndDrawing();
		}
			break;
		case LS_OK:
			loaded = true;
			break;
		}
	}
}

void game_screen(State* state)
{
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);

		EndTextureMode();

		BeginDrawing();
		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);

#ifdef _DEBUG
		DrawFPS(0, 0);
#endif // _DEBUG
		EndDrawing();
	}
}
