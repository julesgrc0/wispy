#pragma once

#define CUBE_SRC_RECT                                                          \
  (Rectangle) { 0, 0, 8, 8 }

typedef enum w_blocktype {
  BLOCK_AIR = 0,
  BLOCK_GRASS,
  BLOCK_DIRT,
  BLOCK_STONE,
  BLOCK_MINERAL,
  BLOCK_MINERAL_OR,
  BLOCK_MINERAL_IRON,
} w_blocktype;

typedef struct w_block {
  w_blocktype type; // 16
} w_block;

typedef struct w_renderblock {
  Rectangle dst;
  Rectangle src;
  w_block block;
  Color light;
} w_renderblock;
