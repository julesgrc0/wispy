#pragma once
#include "../screen/game.h"
#include "../screen/loading.h"
#include "../screen/menu.h"
#include "../stdafx.h"
#include "state.h"

State *init_mainframe();

void destroy_mainframe(State *state);

void loop_mainframe(State *state);