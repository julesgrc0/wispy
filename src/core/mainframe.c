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
	
	state->camera =  (Camera2D){
		.offset = {0},
		.target = {RENDER_WIDTH / 2,RENDER_HEIGHT / 2},
		.zoom = 1.f,
		.rotation = 0
	};

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

	menu_screen(state);
	game_screen(state);
}