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
  button->size = (Vector2){
      .x = MeasureText(text, ctx->font_size),
      .y = ctx->font_size,
  };
  move_button(button, position);
  return button;
}

void move_button(w_guibutton *button, Vector2 position) {
  button->position = (Vector2){
      .x = position.x - (button->size.x / 2),
      .y = position.y - (button->size.y / 2),
  };

  button->rect = (Rectangle){
      .x = button->position.x -
           (button->ctx->margin_width + button->ctx->border_size),
      .y = button->position.y -
           (button->ctx->margin_height + button->ctx->border_size),
      .width = button->size.x +
               2 * (button->ctx->margin_width + button->ctx->border_size),
      .height = button->size.y +
                2 * (button->ctx->margin_height + button->ctx->border_size),
  };
}

bool update_button(w_guibutton *button) {
  bool is_hover =
      CheckCollisionRecs(button->rect, (Rectangle){
                                           .x = GetMousePosition().x,
                                           .y = GetMousePosition().y,
                                           .width = 1,
                                           .height = 1,
                                       });
  Color color = is_hover ? button->hover_color : button->default_color;

  DrawRectangleRoundedLines(button->rect, button->ctx->border_radius, 1.f,
                            button->ctx->border_size, color);
  DrawText(button->text, button->position.x, button->position.y,
           button->ctx->font_size, color);

  return IsMouseButtonDown(MOUSE_LEFT_BUTTON) && is_hover;
}

void destroy_button(w_guibutton *button) {
  if (button) {
    free(button);
  }
}
