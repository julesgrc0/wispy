#pragma once
#define CUBE_W 128 //(RENDER_W/10)
#define CUBE_H 72  //(RENDER_H/10)

#define BLOCK_BRUTE_VALUE_MAX 255
#define BLOCK_TOP_LAYER_H 40

#define CUBE_SRC_RECT \
	(Rectangle) { 0, 0, 8, 8 }

typedef enum BlockTypes
{
	BLOCK_AIR = 0,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_MINERAL,
	BLOCK_MINERAL_OR,
	BLOCK_MINERAL_IRON,
} BlockTypes;

typedef union Block
{
	BlockTypes type; // 16
} Block;

typedef struct RenderBlock
{
	Rectangle dst;
	Rectangle src;
	Block block;

	unsigned int light;
} RenderBlock;
