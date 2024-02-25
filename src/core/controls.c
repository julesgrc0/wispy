#include "controls.h"

w_controls *create_controls() {
  w_controls *kb = (w_controls *)malloc(sizeof(w_controls));
  if (kb == NULL)
    return NULL;
  memset(kb, 0, sizeof(w_controls));
  return kb;
}

void destroy_controls(w_controls *kb) {
  if (kb != NULL)
    free(kb);
}

void update_controls(w_controls *kb) {
#ifdef __ANDROID__
  kb->jump = kb->is_jumping;
  if (!kb->is_breaking) {
    kb->left = kb->joystick.x < -0.5;
    kb->right = kb->joystick.x > 0.5;
  }
#else
  kb->left = IsKeyDown(KEY_LEFT);
  kb->right = IsKeyDown(KEY_RIGHT);
  kb->up = IsKeyDown(KEY_UP);
  kb->down = IsKeyDown(KEY_DOWN);
  kb->jump = IsKeyDown(KEY_SPACE);
  kb->inventory = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_SHIFT);
  kb->shift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
#endif // __ANDROID__
}

void clear_controls(w_controls *kb) {
#ifdef __ANDROID__
  kb->is_jumping = false;
  kb->is_breaking = false;
  kb->joystick = VEC_ZERO;
#endif

  kb->key = 0;
}

#ifdef __ANDROID__
bool check_collision_touch(Vector2 position, float size) {
  return !Vector2Equals(get_collision_touch(position, size), VEC_ZERO);
}

Vector2 get_collision_touch(Vector2 position, float size) {
  const int touch_count = GetTouchPointCount();
  for (int i = 0; i < touch_count; i++) {
    Vector2 point =
        VEC(FORMAT_W(GetTouchPosition(i).x), FORMAT_H(GetTouchPosition(i).y));
    if (CheckCollisionPointCircle(point, position, size)) {
      return point;
    }
  }
  return VEC_ZERO;
}

bool check_collision_touch_rect(Rectangle rect) {
  return !Vector2Equals(get_collision_touch_rect(rect), VEC_ZERO);
}
Vector2 get_collision_touch_rect(Rectangle rect) {
  const int touch_count = GetTouchPointCount();
  for (int i = 0; i < touch_count; i++) {
    Vector2 point =
        VEC(FORMAT_W(GetTouchPosition(i).x), FORMAT_H(GetTouchPosition(i).y));
    if (CheckCollisionPointRec(point, rect)) {
      return point;
    }
  }
  return VEC_ZERO;
}
bool has_touch() { return GetTouchPointCount() > 0; }
#endif