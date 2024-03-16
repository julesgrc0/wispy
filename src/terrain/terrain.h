#pragma once
#include "../stdafx.h"
#include "chunk.h"
#include "chunk_group.h"
#include "chunk_view.h"


typedef struct w_terrain {
    unsigned int position;
    w_chunkgroup* group;
    w_chunkview* view;
} w_terrain;

w_terrain* create_terrain(unsigned int start_position);
void destroy_terrain(w_terrain* terrain);

