#pragma once

#include "../stdafx.h"
#include "../physac.h"


#include "state.h"
#include "../terrain/chunk.h"
#include "../entity/player.h"

#define smooth_camera(camera, player, speed) camera = (camera < player) ? fmin(camera + dt * speed, player) : fmax(camera - dt * speed, player);


typedef struct ControllerThreadData {
	unsigned int active : 1;
	
	Chunk* chunk_next;
	Chunk* chunk_current;
	
	unsigned int position_current : 6;
	unsigned int position_next : 6;

	Camera2D* camera;
	Player* player;
	State* state;

#ifdef _WIN32
	HANDLE handle;
#endif // _WIN32

} ControllerThreadData;


ControllerThreadData* start_controller(State *state, Camera2D* camera, Player* player);
void stop_controller(ControllerThreadData*);

#ifdef _WIN32
DWORD WINAPI controller_thread(LPVOID arg);
#endif // _WIN32