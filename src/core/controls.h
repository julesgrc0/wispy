#pragma once
#include "../gui/gui.h"
#include "../stdafx.h"
#include "view.h"

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
  Vector2 cursor_position;
  Vector2 joystick_position;
  Vector2 break_position;
  Vector2 jump_position;

  float joystick_size;
  float cursor_size;
  float actions_size;

  bool is_breaking;
  bool is_jumping;
#endif
} w_controls;
#pragma pack(pop)

w_controls *create_controls();
void destroy_controls(w_controls *kb);

void update_controls(w_controls *kb);
void clear_controls(w_controls *kb);

#ifdef __ANDROID__
void draw_controls(w_controls *kb);
void draw_control_button(Vector2 position, float joystick_size, Color color);
#endif
