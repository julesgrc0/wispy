#pragma once
#include "../stdafx.h"
#include "block.h"

#define TERRAIN_FOLDER "terrain"
#define TERRAIN_CONFIG "/data.json"

char *get_terrain_path_folder();
char *get_terrain_path_config();
char *get_terrain_path_chunk(unsigned int position);

// TODO: compress chunk files
bool read_chunk_file(unsigned int position, w_block *blocks);
bool write_chunk_file(unsigned int position, w_block *blocks);