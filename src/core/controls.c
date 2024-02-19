#include "controls.h"

w_controls *create_controls() {
  w_controls *kb = (w_controls *)malloc(sizeof(w_controls));
  if (kb == NULL)
    return NULL;
  memset(kb, 0, sizeof(w_controls));

#ifdef __ANDROID__
  kb->joystick_size = PERCENT_H(0.15f);

  float margin = PERCENT_H(0.1);
  kb->joystick_position =
      (Vector2){RENDER_W - margin - (kb->joystick_size / 2),
                RENDER_H - margin - (kb->joystick_size / 2)};
  kb->cursor_position = kb->joystick_position;
  kb->cursor_size = kb->joystick_size / 4;

  kb->break_position = (Vector2){margin + (kb->joystick_size / 2),
                                 RENDER_H - margin - (kb->joystick_size / 2)};
  kb->jump_position = (Vector2){RENDER_W - margin - (kb->joystick_size / 2),
                                margin + (kb->joystick_size / 2)};
  kb->actions_size = kb->joystick_size / 2;
  kb->is_breaking = false;
  kb->is_jumping = false;
#endif

  return kb;
}

void destroy_controls(w_controls *kb) {
  if (kb != NULL)
    free(kb);
}

void update_controls(w_controls *kb) {
  kb->left = IsKeyDown(KEY_LEFT);
  kb->right = IsKeyDown(KEY_RIGHT);
  kb->up = IsKeyDown(KEY_UP);
  kb->down = IsKeyDown(KEY_DOWN);
  kb->jump = IsKeyDown(KEY_SPACE);
  kb->inventory = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_SHIFT);
  kb->shift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
}

void clear_controls(w_controls *kb) { kb->key = 0; }

#ifdef __ANDROID__
void draw_controls(w_controls *kb) {
  draw_control_button(kb->joystick_position, kb->joystick_size, WHITE);

  Vector2 mouse = get_mouse_position();
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (Vector2Distance(mouse, kb->joystick_position) > kb->joystick_size) {
      Vector2 dir = Vector2Subtract(mouse, kb->joystick_position);
      dir = Vector2Normalize(dir);
      dir = Vector2Scale(dir, kb->joystick_size);
      kb->cursor_position = Vector2Add(kb->joystick_position, dir);
    } else {

      kb->cursor_position = mouse;
    }
  } else {
    kb->cursor_position = kb->joystick_position;
  }
  DrawCircle(kb->cursor_position.x, kb->cursor_position.y, kb->cursor_size,
             Fade(GRAY, 0.8f));

  draw_control_button(kb->break_position, kb->actions_size, WHITE);
  draw_control_button(kb->jump_position, kb->actions_size, WHITE);
}

void draw_control_button(Vector2 position, float joystick_size, Color color) {
  DrawCircle(position.x, position.y, joystick_size, Fade(color, 0.8f));
  DrawCircle(position.x, position.y, joystick_size * 0.9, Fade(color, 0.8f));
}
#endif
