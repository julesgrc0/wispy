#include "gui.h"

w_guicontext *create_gui() {
  w_guicontext *ctx = malloc(sizeof(w_guicontext));
  if (ctx == NULL)
    return NULL;

  ctx->border_size = 2;
  ctx->font_size = 20;
  ctx->border_radius = 0.25f;
  ctx->margin_width = 10;
  ctx->margin_height = 5;
  return ctx;
}

void destroy_gui(w_guicontext *ctx) {
  if (ctx) {
    free(ctx);
  }
}
