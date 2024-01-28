#pragma once
#include "../stdafx.h"
#include "block.h"

typedef struct StructureBlock {
  Block *shape;
  unsigned int len : 14;

  unsigned int baseX : 6;
  unsigned int baseY : 8;
} StructureBlock;

Block *push_structure(Block *blocks, unsigned int *count,
                      StructureBlock structure);