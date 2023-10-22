#pragma once

typedef enum BlockTypes {
	B_NONE = 0,
	B_DIRT,
	B_GRASS,
	B_LEAVES,
	B_LOG,
	B_SAND,
	B_STONE
} BlockTypes;

#pragma pack(push,1)
typedef struct Block {
	unsigned int x : 6;   // 2^6 = 64
	unsigned int y : 8;  // 2^8 = 256

	unsigned int active : 1;
	BlockTypes type;
} Block; 
#pragma pack(pop)