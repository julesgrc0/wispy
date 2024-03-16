#pragma once
#include "../../core/state.h"
#include "../../entities/player.h"
#include "../../stdafx.h"
#include "../../terrain/actions/break.h"
#include "../../terrain/chunk.h"
#include "bridge.h"

#include "../../gui/gui.h"

#if defined(WISPY_ANDROID)
#include "../../gui/iconbutton.h"
#include "../../gui/joystick.h"
#endif

void game_screen(w_state *state);
