#pragma once

typedef enum BlockTypes
{
  B_NONE = 0,
  B_GRASS,
  B_DIRT,
  B_STONE
} BlockTypes;

typedef struct Block
{
  unsigned int x : 6; // 2^6 = 64
  unsigned int y : 8; // 2^8 = 256

  BlockTypes type;
} Block;