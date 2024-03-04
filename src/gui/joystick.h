#pragma once
#include "../core/controls.h"
#include "../stdafx.h"
#include "gui.h"

typedef struct w_guijoystick {
  Vector2 position;
  float size;

  Vector2 cursor;
  w_guicontext *ctx;
#if defined(WISPY_ANDROID)
  Vector2 last_position;
#endif
} w_guijoystick;

w_guijoystick *create_joystick(w_guicontext *ctx, Vector2 position, float size);
Vector2 update_joystick(w_guijoystick *js);
void destroy_joystick(w_guijoystick *js);
