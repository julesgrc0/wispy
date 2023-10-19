#include "mainframe.h"

State* init_mainframe()
{
	State* state = malloc(sizeof(State));
	if (state == NULL) return NULL;

	state->loading = LS_DISPLAY;

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

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE);
	SetTargetFPS(0);

	InitWindow(0, 0, "Wispy");

	return state;
}

void destroy_mainframe(State* state)
{
	if (state == NULL) return;
	
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

void load_assets(State* state)
{

	state->render = LoadRenderTexture(960, 540);
	state->src_rnd = (Rectangle){ 0.0f, 0.0f, (float)state->render.texture.width, -(float)state->render.texture.height };
	state->dest_rnd = (Rectangle){ 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };

	// load assets from ressource -> IDR_ASSETS_PACK1
	// LoadImageFromMemory
	//
	// resource pack structure (gzip) :
	// folder/file.ext
	// <file_content>
	// ...

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
