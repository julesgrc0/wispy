#pragma once
#include "../gui/gui.h"
#include "../stdafx.h"
#include "config.h"

#pragma pack(push, 1)
typedef struct w_controls {
  union {
    struct {
      unsigned int inventory : 1;
      unsigned int jump : 1;
      unsigned int left : 1;
      unsigned int right : 1;

      // -- NOT USED --
      unsigned int up : 1;
      unsigned int down : 1;
      unsigned int shift : 1;
      // --------------
    };
    uint8_t key;
  };

#if defined(WISPY_ANDROID)
  Vector2 joystick;
  bool is_jumping;
  bool is_breaking;
#endif

  w_config *cfg;
} w_controls;
#pragma pack(pop)

w_controls *create_controls(w_config *cfg);
void destroy_controls(w_controls *kb);

void update_controls(w_controls *kb);
void clear_controls(w_controls *kb);

#if defined(WISPY_ANDROID)
bool check_collision_touch(Vector2 position, float size);
Vector2 get_nearest_touch(Vector2 position);
Vector2 get_collision_touch(Vector2 position, float size);

bool check_collision_touch_rect(Rectangle rect);
Vector2 get_collision_touch_rect(Rectangle rect);

bool has_touch();
#endif