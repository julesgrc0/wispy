#include "maps.h"

w_menu_view *create_menu_maps(w_state *state) {
  w_menu_view *view = malloc(sizeof(w_menu_view));
  if (!view) {
    return NULL;
  }
  view->update = update_menu_maps;
  view->data = NULL;

  return view;
}

w_menu_state update_menu_maps(w_menu_view *view) { return MENU_STATE_MAPS; }

void destroy_menu_maps(w_menu_view *view) {
  if (!view)
    return;
  if (!view->data) {
    free(view);
    return;
  }

  free(view);
}