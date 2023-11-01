#pragma once

#include "../stdafx.h"

#include "state.h"

#include "../terrain/chunk.h"
#include "../terrain/world.h"

#include "../entity/player.h"

#define smooth_camera(camera, player, speed) camera = (camera < player) ? fmin(camera + dt * speed, player) : fmax(camera - dt * speed, player);

typedef struct BridgeThreadData
{
	unsigned int active : 1;

	Chunk *chunk_current;
	unsigned int position_current : 6;
	BoundingBox camera_view_current;

	Chunk *chunk_next;
	unsigned int position_next : 6;
	BoundingBox camera_view_next;

	Player *player;
	Rectangle player_rect;

	Camera2D *camera;
	World *world;
	State *state;

#ifdef _WIN32
	HANDLE handle;
#endif // _WIN32

} BridgeThreadData;

BridgeThreadData *start_thread(State *state);
void stop_thread(BridgeThreadData *bridge);

#ifdef _WIN32
DWORD WINAPI bridge_thread(LPVOID arg);
#endif // _WIN32