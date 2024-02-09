#pragma once

#include "../stdafx.h"

#include "../entities/player.h"
#include "../terrain/block.h"
#include "../terrain/chunk.h"
#include "../terrain/chunk_group.h"
#include "../terrain/chunk_view.h"

#include "keyboard.h"
#include "state.h"

typedef struct w_bridge {
  bool is_active;

  w_chunkgroup *chunk_group;
  w_chunkview *chunk_view;
  w_keyboard *keyboard;

  w_player *player;

  Camera2D *camera;
  Vector2 camera_target;

#ifdef _WIN32
  HANDLE handle;
#else
  pthread_t handle;
#endif

} w_bridge;

w_bridge *create_bridge();
void destroy_bridge(w_bridge *td);

#ifdef _WIN32
int WINAPI update_bridge(PVOID arg);
#else
void *update_bridge(void *arg);
#endif
