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
    char *data = LoadFileText(config_path);
    if (data == NULL) {
      free(config_path);
      free(cfg);
      return NULL;
    }
    json_object *root = json_tokener_parse(data);
    if (root == NULL) {
      free(data);
      free(config_path);
      free(cfg);
      return NULL;
    }

    load_config_uint(root, cfg, fullscreen);
    load_config_uint(root, cfg, msaa4x);
    load_config_uint(root, cfg, vsync);
    load_config_uint(root, cfg, width);
    load_config_uint(root, cfg, height);
    load_config_uint(root, cfg, max_fps);

    json_object_put(root);
    free(data);
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

  json_object *root = json_object_new_object();
  if (root == NULL) {
    free(config_path);
    return;
  }

  json_object *js_fullscreen = json_object_new_uint64(config->fullscreen);
  json_object *js_msaa4x = json_object_new_uint64(config->msaa4x);
  json_object *js_vsync = json_object_new_uint64(config->vsync);
  json_object *js_width = json_object_new_uint64(config->width);
  json_object *js_height = json_object_new_uint64(config->height);
  json_object *js_max_fps = json_object_new_uint64(config->max_fps);

  json_object_object_add(root, "fullscreen", js_fullscreen);
  json_object_object_add(root, "msaa4x", js_msaa4x);
  json_object_object_add(root, "vsync", js_vsync);
  json_object_object_add(root, "width", js_width);
  json_object_object_add(root, "height", js_height);
  json_object_object_add(root, "max_fps", js_max_fps);

  const char *data =
      json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);
  SaveFileText(config_path, (char *)data);
  json_object_put(root);

  free(config_path);
#endif // !_DEBUG
}
