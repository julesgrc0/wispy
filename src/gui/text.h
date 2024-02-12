#pragma once
#include "gui.h"

typedef struct w_guitext {
  w_guicontext *ctx;
  Vector2 position;
  Color color;
  float font_size;
  char *text;
} w_guitext;

w_guitext *create_text(w_guicontext *ctx, Vector2 position, char *text,
                       float font_size, Color color);

void center_text(w_guitext *text, bool x, bool y);

void update_text(w_guitext *text);

void destroy_text(w_guitext *text);
