#include "config.h"

w_config *load_config() {
  w_config *cfg = malloc(sizeof(w_config));
  if (cfg == NULL)
    return NULL;

  char *config_path = malloc(PATH_LENGTH);
  if (config_path == NULL)
    return NULL;
  config_path[0] = 0;
#if defined(WISPY_ANDROID)
  strcat(config_path, GetAndroidApp()->activity->internalDataPath);
  strcat(config_path, "/");
#else
  strcat(config_path, GetApplicationDirectory());
#endif
  strcat(config_path, CONFIG_NAME);

  if (FileExists(config_path)) {
    char *config_data = LoadFileText(config_path);
    if (config_data == NULL) {
      free(config_path);
      free(cfg);
      return NULL;
    }
    
    json_object *root = json_tokener_parse(config_data);
    if (root == NULL) {
      free(config_path);
      free(cfg);
      UnloadFileText(config_data);
      return NULL;
    }

    json_object *js_fullscreen = json_object_object_get(root, "fullscreen");
    cfg->fullscreen = (unsigned int)json_object_get_uint64(js_fullscreen);
    json_object_put(js_fullscreen);
    

    json_object *js_msaa4x = json_object_object_get(root, "msaa4x");
    cfg->msaa4x = (unsigned int)json_object_get_uint64(js_msaa4x);
    json_object_put(js_msaa4x);
    
    json_object *js_vsync = json_object_object_get(root, "vsync");
    cfg->vsync = (unsigned int)json_object_get_uint64(js_vsync);
    json_object_put(js_vsync);
    
    json_object *js_width = json_object_object_get(root, "width");
    cfg->width = (unsigned int)json_object_get_uint64(js_width);
    json_object_put(js_width);

    json_object *js_height = json_object_object_get(root, "height");
    cfg->height = (unsigned int)json_object_get_uint64(js_height);
    json_object_put(js_height);

    json_object *js_max_fps = json_object_object_get(root, "max_fps");
    cfg->max_fps = (unsigned int)json_object_get_uint64(js_max_fps);
    json_object_put(js_max_fps);
    
    json_object_put(root);
    UnloadFileText(config_data);
  } else {
    memset(cfg, 0, sizeof(w_config));
    cfg->fullscreen = 1;
    cfg->msaa4x = 1;

#if defined(WISPY_ANDROID)
    cfg->vsync = 1;
#endif

#if defined(_DEBUG) && !defined(WISPY_ANDROID)
    cfg->width = RENDER_W;
    cfg->height = RENDER_H;
    cfg->fullscreen = 0;
#endif
  }
  free(config_path);
  return cfg;
}

void save_config(w_config *config) {
  char *config_path = malloc(PATH_LENGTH);
  if (config_path == NULL)
    return;

  config_path[0] = 0;

#if defined(WISPY_ANDROID)
  strcat(config_path, GetAndroidApp()->activity->internalDataPath);
  strcat(config_path, "/");
#else
    strcat(config_path, GetApplicationDirectory());
#endif
  strcat(config_path, CONFIG_NAME);

  json_object *root = json_object_new_object();
  if (root == NULL) {
    free(config_path);
    return;
  }

  json_object *js_fullscreen = json_object_new_uint64(config->fullscreen);
  json_object_object_add(root, "fullscreen", js_fullscreen);

  json_object *js_msaa4x = json_object_new_uint64(config->msaa4x);
  json_object_object_add(root, "msaa4x", js_msaa4x);

  json_object *js_vsync = json_object_new_uint64(config->vsync);
  json_object_object_add(root, "vsync", js_vsync);

  json_object *js_width = json_object_new_uint64(config->width);
  json_object_object_add(root, "width", js_width);

  json_object *js_height = json_object_new_uint64(config->height);
  json_object_object_add(root, "height", js_height);

  json_object *js_max_fps = json_object_new_uint64(config->max_fps);
  json_object_object_add(root, "max_fps", js_max_fps);

  const char *data =
      json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY_TAB);
  SaveFileText(config_path, (char *)data);

  json_object_put(root);
  free(config_path);
}
