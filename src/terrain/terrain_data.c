#include "terrain_data.h"

char *get_terrain_path_folder() {
  char *path = malloc(PATH_LENGTH);
  if (path == NULL) {
    return NULL;
  }
  path[0] = 0;

#if defined(WISPY_ANDROID)
  strcat(path, GetAndroidApp()->activity->internalDataPath);
  strcat(path, "/");
#else
  strcat(path, GetApplicationDirectory());
#endif
  strcat(path, TERRAIN_FOLDER);
  return path;
}

char *get_terrain_path_config() {
  char *path = get_terrain_path_folder();
  if (path == NULL) {
    return NULL;
  }
  strcat(path, TERRAIN_CONFIG);
  return path;
}

char *get_terrain_path_chunk(unsigned int position) {
  char *path = get_terrain_path_folder();
  if (path == NULL) {
    return NULL;
  }

  char position_str[12]; // log10(2^32) + 1 (path) + 1 (end-of-string)
  sprintf(position_str, "/%u", position);
  strcat(path, position_str);

  return path;
}

bool read_chunk_file(unsigned int position, w_block *blocks) {
  char *chunk_path = get_terrain_path_chunk(position);
  if (chunk_path == NULL) {
    return false;
  }
  if (!FileExists(chunk_path)) {
    free(chunk_path);
    return false;
  }

  int in_size = 0;
  char *in_buffer = LoadFileData(chunk_path, &in_size);
  if (in_buffer == NULL) {
    free(chunk_path);
    return false;
  }
  size_t out_size = CHUNK_W * CHUNK_H;
  char *out_buffer = malloc(out_size);
  if (out_buffer == NULL) {
    UnloadFileData(in_buffer);
    free(chunk_path);
    return false;
  }
  if (uncompress((Bytef *)out_buffer, (uLongf *)&out_size,
                 (const Bytef *)in_buffer, (uLong)in_size) != Z_OK) {
    UnloadFileData(in_buffer);
    free(out_buffer);
    free(chunk_path);
    return false;
  }

  for (unsigned int i = 0; i < CHUNK_W * CHUNK_H; i++) {
    blocks[i] = (w_block){
        .is_background = BLOCK_IS_BACKGROUND(out_buffer[i]),
        .type = BLOCK_TYPE(out_buffer[i]),
    };
  }

  UnloadFileData(in_buffer);
  free(out_buffer);
  free(chunk_path);
  return true;
}

bool write_chunk_file(unsigned int position, w_block *blocks) {
  char *chunk_path = get_terrain_path_chunk(position);
  if (chunk_path == NULL) {
    return false;
  }

  size_t in_size = CHUNK_W * CHUNK_H;
  char *in_buffer = malloc(in_size);
  if (in_buffer == NULL) {
    free(chunk_path);
    return false;
  }

  size_t out_size = in_size;
  char *out_buffer = malloc(out_size);
  if (out_buffer == NULL) {
    free(chunk_path);
    free(in_buffer);
    return false;
  }

  for (unsigned int i = 0; i < in_size; i++) {

    if (blocks[i].is_background) {
      in_buffer[i] = BLOCK_SET_BACKGROUND(blocks[i].type);
    } else {
      in_buffer[i] = blocks[i].type;
    }
  }

  if (compress((Bytef *)out_buffer, (uLongf *)&out_size,
               (const Bytef *)in_buffer, (uLong)in_size) != Z_OK) {
    free(chunk_path);
    free(in_buffer);
    return false;
  }

  void *tmp = realloc(out_buffer, out_size);
  if (tmp == NULL) {
    free(chunk_path);
    free(in_buffer);
    free(out_buffer);
    return false;
  }
  out_buffer = tmp;

  SaveFileData(chunk_path, out_buffer, (int)out_size);
  free(chunk_path);
  free(in_buffer);
  return true;
}