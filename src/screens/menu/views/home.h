#pragma once
#include "view.h"

typedef struct w_menu_home_data {
  w_guicontext *ctx;

  w_guibutton *play_button;
  w_guibutton *maps_button;
  w_guibutton *settings_button;
  w_guibutton *exit_button;

  w_guitext *title_text;
  w_guitext *credit_text;
} w_menu_home_data;

w_menu_view *create_menu_home(w_state *state);
w_menu_state update_menu_home(w_menu_view *view);
void destroy_menu_home(w_menu_view *view);