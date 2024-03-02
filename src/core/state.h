#pragma once
#include "../stdafx.h"
#include "config.h"

typedef enum w_framestate {
  FS_DISPLAY,
  FS_LOAD,
  FS_OK,
  FS_FAILED,
  FS_EXIT
} w_framestate;

typedef struct w_state {
  w_framestate state;

  char **textures_id;
  Texture *textures;
  size_t textures_len;

  char **shaders_id;
  Shader *shaders;
  size_t shaders_len;

  Font font;

  RenderTexture render;
  Rectangle rnd_src;
  Vector2 rnd_tl, rnd_tr, rnd_bl, rnd_br;

  w_config *config;

#if defined(WISPY_WINDOWS)
  HINSTANCE hInstance;
#endif

} w_state;

w_state *create_state();
void destroy_state(w_state *state);

void draw_render_texture(w_state *state);
Texture get_texture_by_id(w_state *state, char *id);
Shader get_shader_by_id(w_state *state, char *id);
