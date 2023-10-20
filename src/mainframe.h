#pragma once
#include "stdafx.h"
#include "unpack/unpack.h"

typedef enum LoadingState {
	LS_DISPLAY,
	LS_LOAD,
	LS_OK,
	LS_FAILED,
} LoadingState;

typedef struct State {
	LoadingState loading;

	char** textures_id;
	Texture* textures;
	size_t len;

	Font font;

	RenderTexture render;
	Rectangle src_rnd;
	Rectangle dest_rnd;

#ifdef _WIN32
	HINSTANCE hInstance;
#endif // _WIN32

} State;


State* init_mainframe();
void destroy_mainframe(State* state);

void loop_mainframe(State* state);

Texture get_texture_from_id(State* state, char* id);

void load_assets(State*);
void load_config(State*);

void loading_screen(State* state);
void game_screen(State* state);