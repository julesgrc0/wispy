#pragma once
#include "../stdafx.h"

typedef union w_keyboard {
  struct {
    unsigned int space : 1;
    unsigned int shift : 1;
    unsigned int left : 1;
    unsigned int right : 1;
    unsigned int up : 1;
    unsigned int down : 1;
  };
  unsigned int key : 6;
} w_keyboard;

void update_keyboard(w_keyboard *kb);
void clear_keyboard(w_keyboard *kb);
