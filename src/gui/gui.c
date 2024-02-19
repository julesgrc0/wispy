#include "gui.h"

w_guicontext *create_gui(Vector2 render_size) {
  w_guicontext *ctx = malloc(sizeof(w_guicontext));
  if (ctx == NULL)
    return NULL;

  ctx->render_size = render_size;
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

Vector2 percent_to_pixel(w_guicontext *ctx, Vector2 vec) {
  return (Vector2){.x = ctx->render_size.x * vec.x,
                   .y = ctx->render_size.y * vec.y};
}

Vector2 pixel_to_percent(w_guicontext *ctx, Vector2 vec) {
  return (Vector2){.x = vec.x / ctx->render_size.x,
                   .y = vec.y / ctx->render_size.y};
}
