#pragma once
#include "../stdafx.h"

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

Texture get_texture_by_id(State* state, char* id);
