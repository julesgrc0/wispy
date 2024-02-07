#include "keyboard.h"

void update_keyboard(w_keyboard *kb) {
  kb->left = IsKeyDown(KEY_LEFT);
  kb->right = IsKeyDown(KEY_RIGHT);
  kb->up = IsKeyDown(KEY_UP);
  kb->down = IsKeyDown(KEY_DOWN);
  kb->space = IsKeyDown(KEY_SPACE);
  kb->shift = IsKeyDown(KEY_LEFT_SHIFT);
}

void clear_keyboard(w_keyboard *kb) { kb->key = 0; }
