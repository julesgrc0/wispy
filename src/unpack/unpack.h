#pragma once
#include "../stdafx.h"
#include "../resource.h"

typedef struct AssetItem
{
	char *name;

	char *buffer;
	unsigned int size;
} AssetItem;

char *load_resource(HINSTANCE hInstance, size_t *size);
AssetItem *unpack_assets(HINSTANCE hInstance, size_t *size);