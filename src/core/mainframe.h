#pragma once
#include "../stdafx.h"
#include "state.h"

#include "../screen/loading.h"
#include "../screen/menu.h"
#include "../screen/game.h"

State* init_mainframe();

void destroy_mainframe(State* state);

void loop_mainframe(State* state);