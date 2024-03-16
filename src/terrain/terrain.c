#include "terrain.h"

w_terrain *create_terrain(unsigned int start_position) {

  w_terrain *terrain = malloc(sizeof(w_terrain));
  if (!terrain)
    return NULL;

  terrain->position = start_position;

  char *terrain_path = get_terrain_path_folder();
  if (!DirectoryExists(terrain_path)) {
#if defined(WISPY_WINDOWS)
    if (!CreateDirectoryA(terrain_path, NULL))
#elif defined(WISPY_LINUX)
    if (mkdir(terrain_path, 0777) == -1)
#elif defined(WISPY_ANDROID)
    if (mkdir(terrain_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
#endif
    {
      free(terrain_path);
      free(terrain);
      return NULL;
    }
  }
  free(terrain_path);

  terrain->group = create_chunkgroup(start_position);
  terrain->view = create_chunkview(terrain->group->chunks[0]);
  return terrain;
}

void destroy_terrain(w_terrain *terrain) {
  if (!terrain)
    return;

  destroy_chunkgroup(terrain->group);
  destroy_chunkview(terrain->view);
  free(terrain);
}