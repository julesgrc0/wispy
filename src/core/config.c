#include "config.h"

w_config *load_config() {
  w_config *cfg = malloc(sizeof(w_config));
  if (cfg == NULL)
    return NULL;

#ifndef __ANDROID__
  char *config_path = malloc(MAX_PATH * 2);
  if (config_path == NULL)
    return NULL;

  config_path[0] = 0;
  strcat(config_path, GetApplicationDirectory());
  strcat(config_path, CONFIG_NAME);

  if (FileExists(config_path)) {
    unsigned int size = 0;
    char *data = LoadFileData(config_path, &size);
    cfg = (w_config *)memmove(cfg, data, sizeof(w_config));

    sfree(data);
  } else {
#endif // !__ANDROID__

    cfg->fullscreen = 1;
    cfg->vsync = 0;
    cfg->msaa4x = 1;

    cfg->max_fps = 0;
    cfg->height = 0;
    cfg->width = 0;

#if defined(_DEBUG) && !defined(__ANDROID__)
    cfg->fullscreen = 0;
    cfg->height = RENDER_H;
    cfg->width = RENDER_W;
#endif // _DEBUG

#ifndef __ANDROID__
  }
  free(config_path);
#endif // !__ANDROID__

  return cfg;
}

void save_config(w_config *config) {
#if !defined(_DEBUG) && !defined(__ANDROID__)
  char *config_path = malloc(MAX_PATH * 2);
  if (config_path == NULL)
    return;

  config_path[0] = 0;

  strcat(config_path, GetApplicationDirectory());
  strcat(config_path, CONFIG_NAME);

  SaveFileData(config_path, config, sizeof(w_config));
  free(config_path);
#endif // !_DEBUG
}
