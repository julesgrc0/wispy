#pragma once
#include "../screens/game/game.h"
#include "../screens/loading/loading.h"
#include "../screens/menu/menu.h"
#include "../stdafx.h"
#include "state.h"

w_state *init_mainframe();

void destroy_mainframe(w_state *state);

void loop_mainframe(w_state *state);
