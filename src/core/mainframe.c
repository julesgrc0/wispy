#include "mainframe.h"

w_state *init_mainframe() {
  w_state *state = malloc(sizeof(w_state));
  if (state == NULL)
    return NULL;

  memset(state, 0, sizeof(w_state));

  state->state = F_DISPLAY;
  state->config = load_config();

#ifdef _DEBUG
  SetTraceLogLevel(LOG_ALL);
  SetRandomSeed(0);
  srand(0);
#else
  SetTraceLogLevel(LOG_NONE);
  SetRandomSeed(time(NULL));
  srand(time(NULL));
#endif // _DEBUG

  unsigned int flags = 0;
  if (state->config->vsync) {
    flags |= FLAG_VSYNC_HINT;
  }

  if (state->config->msaa4x) {
    flags |= FLAG_MSAA_4X_HINT;
  }

  if (state->config->vsync) {
    flags |= FLAG_VSYNC_HINT;
  }

  if (state->config->fullscreen) {
    flags |= FLAG_FULLSCREEN_MODE;
  }

  SetConfigFlags(flags);
  SetTargetFPS(state->config->max_fps);

  InitWindow(state->config->width, state->config->height, "Wispy");
  return state;
}

void destroy_mainframe(w_state *state) {
  if (state == NULL)
    return;

  for (size_t i = 0; i < state->len; i++) {
    sfree(state->textures_id[i]);
    UnloadTexture(state->textures[i]);
  }

  sfree(state->textures);
  sfree(state->textures_id);

  UnloadFont(state->font);
  UnloadRenderTexture(state->render);
  CloseWindow();

  save_config(state->config);

  sfree(state->config);
  sfree(state);
}

void loop_mainframe(w_state *state) {
  loading_screen(state);
  if (state->state != F_OK)
    return;

  menu_screen(state);
  if (state->state != F_OK)
    return;

  game_screen(state);
}
