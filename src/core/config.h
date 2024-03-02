#pragma once
#include "../stdafx.h"

#define CONFIG_NAME "config.json"

typedef struct w_config {
  unsigned int fullscreen : 1;
  unsigned int msaa4x : 1;
  unsigned int vsync : 1;

  unsigned int width : 12;
  unsigned int height : 12;

  unsigned int max_fps : 10;

} w_config;

w_config *load_config();
void save_config(w_config *config);
