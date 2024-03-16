#pragma once
#include "../core/controls.h"
#include "../stdafx.h"
#include "gui.h"

typedef struct w_guiiconbutton {
  Vector2 position;
  Texture icon;
  float size;
  w_guicontext *ctx;

} w_guiiconbutton;

w_guiiconbutton *create_iconbutton(w_guicontext *ctx, Vector2 position, float size,
                           Texture texture);
bool update_iconbutton(w_guiiconbutton *act);
void destroy_iconbutton(w_guiiconbutton *act);