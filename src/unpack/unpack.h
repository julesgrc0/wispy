#pragma once
#include "../resource.h"
#include "../stdafx.h"

typedef struct w_asset {
  char *name;

  char *buffer;
  unsigned int size;
} w_asset;

char *load_resource(HINSTANCE hInstance, size_t *size);
w_asset *unpack_assets(HINSTANCE hInstance, size_t *size);
