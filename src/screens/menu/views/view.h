#pragma once

#include "../../../core/state.h"
#include "../../../core/utils/camera.h"

#include "../../../gui/button.h"
#include "../../../gui/gui.h"
#include "../../../gui/text.h"
#include "../../../stdafx.h"

typedef enum w_menu_state {
  MENU_STATE_HOME = 0,
  MENU_STATE_SETTINGS,
  MENU_STATE_MAPS,
  MENU_STATE_PLAY,
  MENU_STATE_EXIT,
} w_menu_state;


typedef struct w_menu_view {
  void* data;
  w_menu_state (*update)(struct w_menu_view* view);
} w_menu_view;