#pragma once
#include "../stdafx.h"


typedef struct w_guicontext {
  float border_size;
  float margin_width;
  float margin_height;
  float font_size;
  float border_radius;
} w_guicontext;

w_guicontext *create_gui();
void destroy_gui(w_guicontext *ctx);

