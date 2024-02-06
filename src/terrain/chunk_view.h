#pragma once
#include "../stdafx.h"
#include "../core/view.h"

#include "block.h"
#include "chunk.h"

typedef struct w_chunkview
{
	w_chunk* active;
	w_chunk* next;

	w_renderblock* blocks;
	unsigned int len; // : 15; // 2^15 = 2^14 * 2

	w_renderblock* next_blocks;
	unsigned int next_len;

	bool lock;
} w_chunkview;


w_chunkview* create_chunk_view();
void update_chunk_view(w_chunkview* chunk_view, w_chunkgroup* grp, Rectangle view);
void swap_chunk_view(w_chunkview* chunk_view);