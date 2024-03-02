#pragma once

#include "../../stdafx.h"
#include "../../entities/player.h"
#include "../../terrain/block.h"
#include "../../terrain/chunk.h"
#include "../../terrain/chunk_group.h"
#include "../../terrain/chunk_view.h"

#include "../../core/utils/camera.h"
#include "../../core/controls.h"
#include "../../core/state.h"

typedef struct w_bridge {
  bool is_active;
  bool force_update;

  w_chunkgroup *chunk_group;
  w_chunkview *chunk_view;
  w_controls *ctrl;

  w_player *player;
  w_camera *camera;

#if defined(WISPY_WINDOWS)
  HANDLE handle;
  LARGE_INTEGER time_start, time_end, time_frequency;
#elif defined(WISPY_LINUX)
  struct timespec time_start, time_end;
  pthread_t handle;
#endif

} w_bridge;

w_bridge *create_bridge();
void destroy_bridge(w_bridge *td);

void physics_update_bridge(w_bridge *td);
void update_bridge(w_bridge *td);

#if defined(WISPY_WINDOWS)
int WINAPI thread_bridge(PVOID arg);
#elif defined(WISPY_LINUX)
void *thread_bridge(void *arg);
#endif
