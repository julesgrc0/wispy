#pragma once
#define CUBE_W 128 //(RENDER_W/10)
#define CUBE_H 72  //(RENDER_H/10)

#define BLOCK_BRUTE_VALUE_MAX 255
#define BLOCK_TOP_LAYER_H 40

#define CUBE_SRC_RECT \
	(Rectangle) { 0, 0, 8, 8 }

typedef enum w_blocktype
{
	BLOCK_AIR = 0,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_MINERAL,
	BLOCK_MINERAL_OR,
	BLOCK_MINERAL_IRON,
} w_blocktype;

typedef union w_block
{
	w_blocktype type; // 16
} w_block;

typedef struct w_renderblock
{
	Rectangle dst;
	Rectangle src;
	w_block block;

	unsigned int light;
} w_renderblock;
