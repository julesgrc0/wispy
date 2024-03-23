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

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
  int jump_key;
  int left_key;
  int right_key;
  int inventory_key;
#elif defined(WISPY_ANDROID)
  Vector2 joystick_position;

  Vector2 break_position;
  Vector2 place_position;

  Vector2 jump_position;

  Vector2 inventory_position;
#endif

} w_config;

w_config *load_config();
void save_config(w_config *config);

Vector2 json_object_get_vec2(json_object * obj);
json_object *json_object_new_vec2(Vector2 vec);