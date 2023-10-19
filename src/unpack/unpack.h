#pragma once
#include "../stdafx.h"
#include "../resource.h"

typedef struct AssetItem {
	char* name;

	char* buffer;
	unsigned int size;
}AssetItem;


int decompress(char* in_buffer, size_t in_size, char** out_buffer, size_t* out_size);
char* load_resource(HINSTANCE hInstance, size_t* size);
AssetItem* unpack_assets(HINSTANCE hInstance, size_t* size);