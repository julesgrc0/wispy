#include "button.h"

w_guibutton *create_button(w_guicontext *ctx, Vector2 position, Color color,
                           char *text) {
  w_guibutton *button = malloc(sizeof(w_guibutton));
  if (button == NULL)
    return NULL;

  button->ctx = ctx;
  button->default_color = color;
  button->hover_color = Fade(color, 0.8f);
  button->text = text;
  button->size =
      VEC((float)MeasureText(text, (int)ctx->font_size), ctx->font_size);
  move_button(button, position);
  return button;
}

void move_button(w_guibutton *button, Vector2 position) {
  button->position =
      VEC(position.x - (button->size.x / 2), position.y - (button->size.y / 2));

  button->rect =
      RECT(button->position.x -
               (button->ctx->margin_width + button->ctx->border_size),
           button->position.y -
               (button->ctx->margin_height + button->ctx->border_size),
           button->size.x +
               2 * (button->ctx->margin_width + button->ctx->border_size),
           button->size.y +
               2 * (button->ctx->margin_height + button->ctx->border_size), );
}

bool update_button(w_guibutton *button) {
#if defined(WISPY_ANDROID)
  bool is_hover = has_touch() && check_collision_touch_rect(button->rect);
#else
  bool is_hover = CheckCollisionRecs(
      button->rect, RECT(FORMAT_W(GetMouseX()), FORMAT_H(GetMouseY()), 1, 1));
#endif

  Color color = is_hover ? button->hover_color : button->default_color;

  DrawRectangleRoundedLines(button->rect, button->ctx->border_radius, 1,
                            button->ctx->border_size, color);
  DrawText(button->text, (int)button->position.x, (int)button->position.y,
           (int)button->ctx->font_size, color);

#if defined(WISPY_ANDROID)
  return is_hover;
#else
  return IsMouseButtonDown(MOUSE_LEFT_BUTTON) && is_hover;
#endif
}

void destroy_button(w_guibutton *button) {
  if (button) {
    free(button);
  }
}
