#include "settings.h"

w_menu_view *create_menu_settings(w_state *state) {
  w_menu_view *view = malloc(sizeof(w_menu_view));
  if (!view) {
    return NULL;
  }
  view->update = update_menu_settings;
  view->data = NULL;

  return view;
}

w_menu_state update_menu_settings(w_menu_view *view) {
  return MENU_STATE_SETTINGS;
}

void destroy_menu_settings(w_menu_view *view) {
  if (!view)
    return;
  if (!view->data) {
    free(view);
    return;
  }

  free(view);
}