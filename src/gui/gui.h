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

/*
TODO: idea for a gui system

<frame

  width="X  <%|px>"
  height="Y <%|px>"

  x="X <%|px>"
  y="Y <%|px>"

  border="X <%|px>"
  border-radius="X <%|px>"

  padding="X <%|px>"
>
  ... (frames)
</frame>

typedef struct w_guiframe w_guiframe;

typedef struct w_guiframe {
  float width;
  float height;

  float x;
  float y;

  float border;
  float border_radius;

  float padding;

  Color background_color;
  Color border_color;

  char *text;
  int font_size;

  Texture texture;

  size_t len;
  w_guiframe *frames;
};


void calculate_frame(w_guiframe* root);


*/