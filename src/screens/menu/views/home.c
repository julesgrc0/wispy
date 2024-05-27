#include "home.h"

w_menu_view *create_menu_home(w_state *state) {
  w_menu_home_data *data = (w_menu_home_data *)malloc(sizeof(w_menu_home_data));
  if (!data) {
    return NULL;
  }

  w_menu_view *view = (w_menu_view *)malloc(sizeof(w_menu_view));
  if (!view) {
    free(data);
    return NULL;
  }

  view->update = update_menu_home;
  view->data = data;

  data->ctx = create_gui();
  data->ctx->font_size = 25;
  data->ctx->margin_height = 10;
  data->ctx->margin_width = 40;

  data->play_button = create_button(
      data->ctx, VEC(PERCENT_W(0.5f), PERCENT_H(0.45f)), WHITE, "Play");
  data->settings_button = create_button(
      data->ctx,
      VEC(.x = data->play_button->position.x + data->play_button->size.x / 2,
          .y =
              data->play_button->rect.y + data->play_button->rect.height * 2, ),
      WHITE, "Settings");
  data->settings_button->default_color = Fade(WHITE, 0.5);
  data->settings_button->hover_color = Fade(WHITE, 0.5);

  data->exit_button = create_button(
      data->ctx,
      VEC(.x = data->play_button->position.x + data->play_button->size.x / 2,
          .y = data->settings_button->rect.y +
               data->settings_button->rect.height * 2),
      WHITE, "Exit");

  data->title_text = create_text(
      data->ctx, VEC(PERCENT_W(0.5f), PERCENT_H(0.2f)), "Wispy", 120, WHITE);
  center_text(data->title_text, true, true);

  data->credit_text = create_text(
      data->ctx,
      Vector2Add(data->title_text->position,
                 VEC(0, data->title_text->font_size + 10)),
      (char *)TextFormat("made by @julesgrc0 - %s", WISPY_VERSION), 20, WHITE);

  return view;
}

w_menu_state update_menu_home(w_menu_view *view) {

  w_menu_state state = MENU_STATE_HOME;
  w_menu_home_data *data = (w_menu_home_data *)view->data;

  if (update_button(data->play_button)) {
    state = MENU_STATE_PLAY;
  }
  if (update_button(data->settings_button)) {
    state = MENU_STATE_SETTINGS;
  }
  if (update_button(data->exit_button)) {
    state = MENU_STATE_EXIT;
  }

  update_text(data->title_text);
  update_text(data->credit_text);

  return state;
}

void destroy_menu_home(w_menu_view *view) {
  if (!view)
    return;

  if (!view->data) {
    free(view);
    return;
  }

  w_menu_home_data *data = (w_menu_home_data *)view->data;

  destroy_text(data->credit_text);
  destroy_text(data->title_text);

  destroy_button(data->play_button);
  destroy_button(data->settings_button);
  destroy_button(data->exit_button);

  destroy_gui(data->ctx);

  free(data);
  free(view);
}