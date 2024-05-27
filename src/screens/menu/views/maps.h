#pragma once
#include "view.h"

w_menu_view* create_menu_maps(w_state* state);
w_menu_state update_menu_maps(w_menu_view* view);
void destroy_menu_maps(w_menu_view* view);