#pragma once
#include "../stdafx.h"

#pragma pack(push, 1)
typedef union w_keyboard {
  struct {
    unsigned int inventory : 1;
    unsigned int jump : 1;
    unsigned int left : 1;
    unsigned int right : 1;
  };
  uint8_t key;
} w_keyboard;
#pragma pack(pop)

void update_keyboard(w_keyboard *kb);
void clear_keyboard(w_keyboard *kb);
