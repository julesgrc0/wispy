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
  DrawCircle(js->position.x, js->position.y, js->size, Fade(WHITE, 0.3f));
  DrawCircle(js->position.x, js->position.y, js->size * 0.9f,
             Fade(WHITE, 0.3f));

#ifdef __ANDROID__
  if (has_touch()) {
    Vector2 mouse = VEC(FORMAT_W(GetMouseX()), FORMAT_H(GetMouseY()));
    Vector2 touch = get_collision_touch(js->position, js->size);
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
  } else {
    js->cursor = js->position;
  }
  DrawCircle(js->cursor.x, js->cursor.y, js->size / 4, Fade(BLACK, 0.5f));

  Vector2 dir = Vector2Subtract(js->cursor, js->position);
  dir = Vector2Normalize(dir);
  return dir;
}

void destroy_joystick(w_guijoystick *js) {
  if (js) {
    free(js);
  }
}
