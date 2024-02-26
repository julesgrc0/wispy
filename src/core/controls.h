#pragma once
#include "../gui/gui.h"
#include "../stdafx.h"

#pragma pack(push, 1)
typedef struct w_controls {
  union {
    struct {
      unsigned int inventory : 1;
      unsigned int jump : 1;
      unsigned int left : 1;
      unsigned int right : 1;
      unsigned int up : 1;
      unsigned int down : 1;
      unsigned int shift : 1;
    };
    uint8_t key;
  };

#ifdef __ANDROID__
  Vector2 joystick;
  bool is_jumping;
  bool is_breaking;
#endif
} w_controls;
#pragma pack(pop)

w_controls *create_controls();
void destroy_controls(w_controls *kb);

void update_controls(w_controls *kb);
void clear_controls(w_controls *kb);

#ifdef __ANDROID__
bool check_collision_touch(Vector2 position, float size);
Vector2 get_collision_touch(Vector2 position, float size);

bool check_collision_touch_rect(Rectangle rect);
Vector2 get_collision_touch_rect(Rectangle rect);

bool has_touch();
#endif