#pragma once
#include "../screen/game.h"
#include "../screen/loading.h"
#include "../screen/menu.h"
#include "../stdafx.h"
#include "state.h"

w_state *init_mainframe();

void destroy_mainframe(w_state *state);

void loop_mainframe(w_state *state);
