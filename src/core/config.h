#pragma once
#include "../stdafx.h"

#define CONFIG_NAME "config.json"

#define load_config_uint(jsobj, config, name)                                  \
  json_object *js_##name = json_object_object_get(jsobj, #name);               \
  config->##name = (unsigned int)json_object_get_uint64(js_##name);            \
  json_object_put(js_##name);

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
