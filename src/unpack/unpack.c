#include "unpack.h"

char *load_resource(HINSTANCE hInstance, size_t *size) {
#ifdef _WIN32

  HRSRC hResInfo = FindResourceA(hInstance, MAKEINTRESOURCE(IDR_ASSETS_PACK1),
                                 "ASSETS_PACK");
  if (!hResInfo) {
    return NULL;
  }

  HGLOBAL hResData = LoadResource(hInstance, hResInfo);
  if (!hResData) {
    return NULL;
  }

  *size = SizeofResource(hInstance, hResInfo);
  char *data = LockResource(hResData);

  FreeResource(hResData);

  return data;
#else
  return NULL;  // TODO
#endif
}

AssetItem *unpack_assets(HINSTANCE hInstance, size_t *size) {
  *size = 0;
  size_t in_size = 0;
  char *in_buffer = load_resource(hInstance, &in_size);

  if (in_buffer == NULL) return NULL;

  size_t out_size = in_size * 2;
  char *out_buffer = malloc(out_size);
  if (!out_buffer) return NULL;

  if (uncompress(out_buffer, (uLongf *)&out_size, in_buffer, (uLong)in_size) !=
      Z_OK) {
    sfree(out_buffer);
    return NULL;
  }
  out_buffer = realloc(out_buffer, out_size);

  AssetItem *items = malloc(sizeof(AssetItem));
  size_t len = 0;
  if (items == NULL) {
    sfree(out_buffer);
    return NULL;
  }

  size_t index = 0;
  while (index < out_size) {
    AssetItem item = {0};
    size_t current_size = 0;

    item.name = malloc(0);
    do {
      item.name = realloc(item.name, current_size + 1);
      item.name[current_size] = out_buffer[index];

      current_size++;
      index++;

      if (out_buffer[index - 1] == 0) break;
    } while (index < out_size);
    current_size = 0;

    char file_size_bytes[sizeof(unsigned int)];
    memcpy(file_size_bytes, out_buffer + index, sizeof(unsigned int));
    index += sizeof(unsigned int);

    item.size = *(unsigned int *)file_size_bytes;

    item.buffer = malloc(item.size);
    memcpy(item.buffer, out_buffer + index, item.size);
    index += item.size;

    items = realloc(items, (len + 1) * sizeof(AssetItem));
    memcpy(&items[len], &item, sizeof(AssetItem));
    len++;
  }

  sfree(out_buffer);
  *size = len;

  return items;
}
