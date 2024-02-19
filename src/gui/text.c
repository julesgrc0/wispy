#include "text.h"

w_guitext *create_text(w_guicontext *ctx, Vector2 position, char *text,
                       float font_size, Color color) {
  w_guitext *tx = malloc(sizeof(w_guitext));
  if (tx == NULL)
    return NULL;

  tx->text = text;
  tx->position = position;
  tx->color = color;
  tx->font_size = font_size;
  tx->ctx = ctx;

  return tx;
}

void center_text(w_guitext *text, bool x, bool y) {
  if (x) {
    text->position.x -= MeasureText(text->text, text->font_size) / 2;
  }
  if (y) {
    text->position.y -= text->font_size / 2;
  }
}

void update_text(w_guitext *text) {
  DrawText(text->text, text->position.x, text->position.y, text->font_size,
           text->color);
}

void destroy_text(w_guitext *text) {
  if (text) {
    free(text);
  }
}
