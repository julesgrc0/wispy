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
  return NULL; // TODO
#endif
}

w_asset *unpack_assets(HINSTANCE hInstance, size_t *size) {
  *size = 0;
  size_t in_size = 0;

  if (hInstance == NULL)
    return NULL;
  char *in_buffer = load_resource(hInstance, &in_size);

  if (in_buffer == NULL)
    return NULL;

  size_t out_size = in_size * 2;
  char *out_buffer = malloc(out_size);
  if (!out_buffer)
    return NULL;

  if (uncompress(out_buffer, (uLongf *)&out_size, in_buffer, (uLong)in_size) !=
      Z_OK) {
    sfree(out_buffer);
    return NULL;
  }
  void *new_outbuff = realloc(out_buffer, out_size);
  if (new_outbuff == NULL) {
    sfree(out_buffer);
    return NULL;
  }
  out_buffer = new_outbuff;

  w_asset *items = malloc(sizeof(w_asset));
  if (items == NULL) {
    sfree(out_buffer);
    return NULL;
  }

  size_t items_len = 0;
  if (items == NULL) {
    sfree(out_buffer);
    return NULL;
  }

  size_t index = 0;
  while (index < out_size) {
    w_asset item = {0};
    size_t current_size = 0;

    item.name = malloc(0);
    do {
      void *new_name = realloc(item.name, current_size + 1);
      if (new_name == NULL) {
        sfree(item.name);
        sfree(out_buffer);

        for (size_t i = 0; i < items_len; i++) {
          sfree(items[i].name);
          sfree(items[i].buffer);
        }
        sfree(items);

        return NULL;
      }
      item.name = new_name;
      item.name[current_size] = out_buffer[index];

      current_size++;
      index++;

      if (out_buffer[index - 1] == 0)
        break;
    } while (index < out_size);
    current_size = 0;

    char file_size_bytes[sizeof(unsigned int)];
    memcpy(file_size_bytes, out_buffer + index, sizeof(unsigned int));
    index += sizeof(unsigned int);

    item.size = *(unsigned int *)file_size_bytes;
    item.buffer = malloc(item.size);
    memcpy(item.buffer, out_buffer + index, item.size);
    index += item.size;

    void *new_items = realloc(items, (items_len + 1) * sizeof(w_asset));
    if (new_items == NULL) {
      sfree(item.name);
      sfree(item.buffer);
      sfree(out_buffer);

      for (size_t i = 0; i < items_len; i++) {
        sfree(items[i].name);
        sfree(items[i].buffer);
      }
      sfree(items);

      return NULL;
    }

    items = new_items;
    memcpy(items + items_len, &item, sizeof(w_asset));
    items_len++;
  }

  sfree(out_buffer);
  *size = items_len;

  return items;
}
