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
    strcat(config_path, GetApplicationDirectory());
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

  char position_str[12]; // log10(2^32) + 1 + 1
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

  int size = 0;
  char *data = LoadFileData(chunk_path, &size);

  if (size != CHUNK_W * CHUNK_H) {
    UnloadFileData(data);
    return false;
  }

  for (unsigned int i = 0; i < CHUNK_W * CHUNK_H; i++) {
    blocks[i] = (w_block){
        .is_background = BLOCK_IS_BACKGROUND(data[i]),
        .type = BLOCK_TYPE(data[i]),
    };
  }

  UnloadFileData(data);
  free(chunk_path);
  return true;
}

bool write_chunk_file(unsigned int position, w_block *blocks) {
  char *chunk_path = get_terrain_path_chunk(position);
  if (chunk_path == NULL) {
    return false;
  }

  char *data = malloc(CHUNK_W * CHUNK_H);
  if (data == NULL) {
    free(chunk_path);
    return false;
  }

  for (unsigned int i = 0; i < CHUNK_W * CHUNK_H; i++) {

    if (blocks[i].is_background) {
      data[i] = BLOCK_SET_BACKGROUND(data[i]);
    } else {
      data[i] = blocks[i].type;
    }
  }

  SaveFileData(chunk_path, data, CHUNK_W * CHUNK_H);
  free(chunk_path);
  free(data);
  return true;
}