#pragma once
#include "../core/view.h"
#include "../core/controls.h"
#include "../stdafx.h"
#include "gui.h"

typedef struct w_guiaction {
  Vector2 position;
  Texture icon;
  float size;
  w_guicontext *ctx;

} w_guiaction;

w_guiaction *create_action(w_guicontext *ctx, Vector2 position, float size,
                           Texture texture);
bool update_action(w_guiaction *act);
void destroy_action(w_guiaction *act);