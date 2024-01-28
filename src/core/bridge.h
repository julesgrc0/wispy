#pragma once

#include "../entities/player.h"
#include "../stdafx.h"
#include "../terrain/chunk.h"
#include "../terrain/world.h"
#include "state.h"

#define smooth_camera(camera, player, speed)                     \
  camera = (camera < player) ? fmin(camera + dt * speed, player) \
                             : fmax(camera - dt * speed, player);

typedef struct BridgeThreadData {
  unsigned int active : 1;

  World *world;
  BoundingBox view_current;
  BoundingBox view_next;

  Player *player;
  Rectangle player_rect;

  Camera2D *camera;
  State *state;

#ifdef _WIN32
  HANDLE handle;
#endif  // _WIN32

} BridgeThreadData;

BridgeThreadData *start_thread(State *state);
void stop_thread(BridgeThreadData *bridge);

#ifdef _WIN32
DWORD WINAPI bridge_thread(LPVOID arg);
#endif  // _WIN32