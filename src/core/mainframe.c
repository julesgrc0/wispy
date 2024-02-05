#include "mainframe.h"

State *init_mainframe() {
  State *state = malloc(sizeof(State));
  if (state == NULL) return NULL;

  memset(state, 0, sizeof(State));

  state->loading = LS_DISPLAY;

  state->len = 0;
  state->textures = NULL;
  state->textures_id = NULL;
  state->font = (Font){0};
  state->render = (RenderTexture){0};
  state->src_rnd = (Rectangle){0};
  state->dest_rnd = (Rectangle){0};

  state->config = load_config();

#ifdef _DEBUG
  SetTraceLogLevel(LOG_ALL);
#else
  SetTraceLogLevel(LOG_NONE);
#endif  // _DEBUG

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

void destroy_mainframe(State *state) {
  if (state == NULL) return;

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

void loop_mainframe(State *state) {
  loading_screen(state);
  if (state->loading != LS_OK) return;

  menu_screen(state);
  game_screen(state);
}