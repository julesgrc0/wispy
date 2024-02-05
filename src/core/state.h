#pragma once
#include "../stdafx.h"

#define CONFIG_NAME "config.dat"

typedef enum LoadingState
{
  LS_DISPLAY,
  LS_LOAD,
  LS_OK,
  LS_FAILED,
} LoadingState;

typedef struct Config
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
} Config;

typedef struct State
{
  LoadingState loading;

  char **textures_id;
  Texture *textures;
  size_t len;

  Font font;

  RenderTexture render;
  Rectangle src_rnd;
  Rectangle dest_rnd;

  Config *config;

#ifdef _WIN32
  HINSTANCE hInstance;
#endif // _WIN32

} State;

Config *load_config();
void save_config(Config *config);

Texture get_texture_by_id(State *state, char *id);
