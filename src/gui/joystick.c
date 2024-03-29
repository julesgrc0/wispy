#include "joystick.h"

w_guijoystick *create_joystick(w_guicontext *ctx, Vector2 position,
                               float size) {
  w_guijoystick *joystick = malloc(sizeof(w_guijoystick));
  if (joystick == NULL)
    return NULL;

  memset(joystick, 0, sizeof(w_guijoystick));
  joystick->ctx = ctx;
  joystick->position = Vector2SubtractValue(position, size);
  joystick->size = size;

  joystick->cursor = joystick->position;
  return joystick;
}

Vector2 update_joystick(w_guijoystick *js) {
  DrawCircleV(js->position, js->size, Fade(WHITE, 0.3f));
  DrawCircleV(js->position, js->size * 0.9f, Fade(WHITE, 0.3f));

#if defined(WISPY_ANDROID)
  if (has_touch()) {
    Vector2 mouse =
        get_nearest_touch(Vector2SubtractValue(js->position, js->size / 2.f));
    Vector2 touch = get_collision_touch(js->position, js->size);
    if (Vector2Equals(js->last_position, VEC_ZERO)) {
      js->last_position = touch;
    } else if (Vector2Distance(js->last_position, mouse) > js->size * 2) {
    } else {
#else
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    Vector2 mouse = VEC(FORMAT_W(GetMouseX()), FORMAT_H(GetMouseY()));
    Vector2 touch = CheckCollisionCircles(mouse, 1, js->position, js->size)
                        ? mouse
                        : VEC_ZERO;
#endif

      if (Vector2Equals(js->cursor, js->position) &&
          !Vector2Equals(touch, VEC_ZERO)) {
        js->cursor = touch;
      } else if (!Vector2Equals(js->cursor, js->position)) {
        Vector2 mtouch = Vector2Equals(touch, VEC_ZERO) ? mouse : touch;
        if (Vector2Distance(mtouch, js->position) > js->size) {
          Vector2 dir = Vector2Subtract(mtouch, js->position);
          dir = Vector2Normalize(dir);
          dir = Vector2Scale(dir, js->size);
          js->cursor = Vector2Add(js->position, dir);
        } else {
          js->cursor = mtouch;
        }
      }
#if defined(WISPY_ANDROID)
    }
#endif
  } else {
    js->cursor = js->position;
#if defined(WISPY_ANDROID)
    js->last_position = VEC_ZERO;
#endif
  }
  DrawCircleV(js->cursor, js->size / 4, Fade(BLACK, 0.5f));

  Vector2 dir = Vector2Subtract(js->cursor, js->position);
  dir = Vector2Normalize(dir);
  return dir;
}

void destroy_joystick(w_guijoystick *js) {
  if (js) {
    free(js);
  }
}
