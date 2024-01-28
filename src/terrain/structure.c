#include "structure.h"

Block *push_structure(Block *blocks, unsigned int *count,
                      StructureBlock structure) {
  Block *new_blocks = malloc(count + structure.len);  // max possible size
  if (!new_blocks) return NULL;

  for (size_t i = 0; i < count; i++) {
    // TODO: add the structure to the new_blocks matrix
  }

  // realloc to the correct size
  return new_blocks;
}
