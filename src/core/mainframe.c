#include "mainframe.h"

w_state *init_mainframe() {
  w_state *state = create_state();
  if (state == NULL)
    return NULL;

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
  if (state->config->msaa4x) {
    flags |= FLAG_MSAA_4X_HINT;
  }

  if (state->config->vsync) {
    flags |= FLAG_VSYNC_HINT;
  }

  if (state->config->fullscreen) {
    flags |= FLAG_FULLSCREEN_MODE;
  }

  InitWindow(state->config->width, state->config->height, "Wispy");

  SetConfigFlags(flags);
  SetTargetFPS(state->config->max_fps);

  state->rnd_src = RECT(0, RENDER_H, RENDER_W, -RENDER_H);

  state->rnd_tl = VEC_ZERO;
  state->rnd_tr = VEC((float)GetScreenWidth(), 0.f);
  state->rnd_bl = VEC(0.f, (float)GetScreenHeight());
  state->rnd_br = VEC((float)GetScreenWidth(), (float)GetScreenHeight());

  return state;
}

void destroy_mainframe(w_state *state) {
  if (state == NULL)
    return;

  for (size_t i = 0; i < state->textures_len; i++) {
    sfree(state->textures_id[i]);
    UnloadTexture(state->textures[i]);
  }
  sfree(state->textures);
  sfree(state->textures_id);

  for (size_t i = 0; i < state->shaders_len; i++) {
    sfree(state->shaders_id[i]);
    UnloadShader(state->shaders[i]);
  }
  sfree(state->shaders);
  sfree(state->shaders_id);

  UnloadFont(state->font);
  UnloadRenderTexture(state->render);
  CloseWindow();

  save_config(state->config);
  destroy_state(state);
}

void loop_mainframe(w_state *state) {
  loading_screen(state);
  if (state->state != FS_OK)
    return;

  menu_screen(state);
  if (state->state != FS_OK)
    return;

  game_screen(state);
}
