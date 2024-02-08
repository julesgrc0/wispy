#pragma once
#include "../stdafx.h"

#pragma pack(push, 1)
typedef union w_keyboard {
  struct {
    unsigned int space : 1;
    unsigned int shift : 1;
    unsigned int left : 1;
    unsigned int right : 1;
  };
  unsigned int key : 4;
} w_keyboard;
#pragma pack(pop)

void update_keyboard(w_keyboard *kb);
void clear_keyboard(w_keyboard *kb);
