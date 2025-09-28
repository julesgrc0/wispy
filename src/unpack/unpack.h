#pragma once
#include "../resource.h"
#include "../stdafx.h"

#define MAX_UNCOMPRESSE_TRY 4

#if defined(WISPY_WINDOWS)
#define RESOURCE_NAME "ASSETS_PACK"
#elif defined(WISPY_LINUX)
extern char w_binary___tools_resource_pack_start;
extern char w_binary___tools_resource_pack_end;
#elif defined(WISPY_MACOS)
#include "resource.pack.h"
#elif defined(WISPY_ANDROID)
#define RESOURCE_NAME "resource.pack"
#endif

typedef struct w_asset {
  char *name;

  char *buffer;
  unsigned int size;
} w_asset;

#if defined(WISPY_WINDOWS)
char *load_resource(HINSTANCE hInstance, size_t *size);
w_asset *unpack_assets(HINSTANCE hInstance, size_t *size);
#else
w_asset *unpack_assets(size_t *size);
#endif