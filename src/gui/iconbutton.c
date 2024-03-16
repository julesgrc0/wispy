#include "iconbutton.h"

w_guiiconbutton *create_iconbutton(w_guicontext *ctx, Vector2 position, float size,
                           Texture texture) {
  w_guiiconbutton *iconbutton = malloc(sizeof(w_guiiconbutton));
  if (iconbutton == NULL)
    return NULL;
  memset(iconbutton, 0, sizeof(w_guiiconbutton));
  iconbutton->ctx = ctx;
  iconbutton->position = Vector2SubtractValue(position, size);
  iconbutton->size = size;
  iconbutton->icon = texture;

  return iconbutton;
}

bool update_iconbutton(w_guiiconbutton *act) {
#if defined(WISPY_ANDROID)
  bool clicked = check_collision_touch(act->position, act->size);
#else
  bool clicked =
      IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
      CheckCollisionPointCircle(GetMousePosition(), act->position, act->size);
#endif

  DrawCircleV(act->position, act->size, Fade(clicked ? BLACK : WHITE, 0.3f));
  DrawCircleV(act->position, act->size * 0.9f,
              Fade(clicked ? BLACK : WHITE, 0.3f));
  DrawTexturePro(
      act->icon,
      RECT(0.f, 0.f, (float)act->icon.width, (float)act->icon.height),
      RECT(act->position.x - act->size / 2, act->position.y - act->size / 2,
           act->size, act->size),
      VEC_ZERO, 0, BLACK);

  return clicked;
}

void destroy_iconbutton(w_guiiconbutton *act) {
  if (act) {
    free(act);
  }
}
