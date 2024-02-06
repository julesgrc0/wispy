#pragma once
#include "../stdafx.h"

#define CONFIG_NAME "config.dat"

typedef enum w_loadstatus
{
  LS_DISPLAY,
  LS_LOAD,
  LS_OK,
  LS_FAILED,
} w_loadstatus;

typedef struct w_config
{
  unsigned int fullscreen : 1;
  unsigned int msaa4x : 1;
  unsigned int vsync : 1;

  unsigned int width : 12;
  unsigned int height : 12;

  unsigned int max_fps : 10;
  unsigned int max_chunks : 10;

  /*
  unsigned int render_size : 12;
  unsigned int render_distance : 6;

  unsigned int max_render_block : 8;

  unsigned int block_size : 8;
  */
} w_config;

typedef struct w_state
{
  w_loadstatus loading;

  char **textures_id;
  Texture *textures;
  size_t len;

  Font font;

  RenderTexture render;
  Rectangle src_rnd;
  Rectangle dest_rnd;

  w_config *config;

#ifdef _WIN32
  HINSTANCE hInstance;
#endif // _WIN32

} w_state;

w_config *load_config();
void save_config(w_config *config);

Texture get_texture_by_id(w_state *state, char *id);
