#pragma once
#include "../resource.h"
#include "../stdafx.h"

typedef struct AssetItem
{
  char *name;

  char *buffer;
  unsigned int size;
} AssetItem;

char *load_resource(HINSTANCE hInstance, size_t *size);
AssetItem *unpack_assets(HINSTANCE hInstance, size_t *size);