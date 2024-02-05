#pragma once

#include "../stdafx.h"
#include "../entities/player.h"
#include "../terrain/chunk.h"
#include "state.h"

#define smooth_camera(camera, player, speed)                       \
	camera = (camera < player) ? fmin(camera + dt * speed, player) \
							   : fmax(camera - dt * speed, player);

#define PHYSICS_TICK (1.f / 24.f)

typedef struct ThreadData
{
	bool request_update;
	bool request_swap;

	bool is_active;
	bool is_locked;

	ChunkGroup *chunkGroup;
	ChunkView *chunkView;
	Camera2D *camera;
	Player *player;

#ifdef _WIN32
	LARGE_INTEGER time_start, time_end, time_frequency;
	HANDLE handle;
#endif
} ThreadData;

ThreadData *start_threadbridge(Player *player);
void stop_threadbridge(ThreadData *td);

void physics_update(ThreadData *td);
int WINAPI update_thread(PVOID arg);