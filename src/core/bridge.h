#pragma once

#include "../stdafx.h"
#include "../entities/player.h"

#include "../terrain/block.h"
#include "../terrain/chunk.h"
#include "../terrain/chunk_view.h"

#include "state.h"

#define smooth_camera(camera, player, speed)                       \
	camera = (camera < player) ? fmin(camera + dt * speed, player) \
							   : fmax(camera - dt * speed, player);

#define PHYSICS_TICK (1.f / 24.f)

typedef union w_keyboard {
	struct
	{
		unsigned int space : 1;
		unsigned int shift : 1;
		unsigned int left  : 1;
		unsigned int right : 1;
		unsigned int up    : 1;
		unsigned int down  : 1;
	};
	unsigned int key : 6;
} w_keyboard;


typedef struct w_bridgedata
{
	bool request_swap;
	bool is_active;

	w_chunkgroup *chunk_group;
	w_chunkview *chunk_view;
	w_player *player;
	w_keyboard *keyboard;

	Camera2D *camera;

#ifdef _WIN32
	LARGE_INTEGER time_start, time_end, time_frequency;
	HANDLE handle;
#endif

} w_bridgedata;


w_bridgedata *start_threadbridge(w_player *player);
void stop_threadbridge(w_bridgedata *td);

void update_keyboard(w_keyboard *kb);
void clear_keyboard(w_keyboard*kb);

#ifdef _WIN32
int WINAPI update_bridgethread(PVOID arg);
#else
void *update_bridgethread(void *arg);
#endif