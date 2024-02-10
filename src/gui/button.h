#pragma once
#include "gui.h"

typedef struct w_guibutton {
  w_guicontext *ctx;
  Vector2 position;
  Vector2 size;
  Color default_color;
  Color hover_color;
  char *text;

  Rectangle rect;
} w_guibutton;

w_guibutton *create_button(w_guicontext *ctx, Vector2 position, Color color,
                           char *text);

bool update_button(w_guibutton *button);

void destroy_button(w_guibutton *button);
