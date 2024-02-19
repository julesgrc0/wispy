#pragma once
#include "../stdafx.h"

#define PERCENT_W(x) RENDER_W *x
#define PERCENT_H(x) RENDER_H *x

typedef struct w_guicontext {
  Vector2 render_size;
  float border_size;
  float margin_width;
  float margin_height;
  float font_size;
  float border_radius;
} w_guicontext;

w_guicontext *create_gui(Vector2 render_size);
void destroy_gui(w_guicontext *ctx);

Vector2 percent_to_pixel(w_guicontext *ctx, Vector2 vec);
Vector2 pixel_to_percent(w_guicontext *ctx, Vector2 vec);
