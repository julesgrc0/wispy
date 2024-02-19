#pragma once
#include "../core/bridge.h"
#include "../core/state.h"
#include "../entities/player.h"
#include "../stdafx.h"
#include "../terrain/break.h"
#include "../terrain/chunk.h"

#include "../gui/gui.h"
#include "../gui/joystick.h"
#include "../gui/action.h"

#define DAY_DURATION 60.0f

void game_screen(w_state *state);
