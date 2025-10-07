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

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX) || defined(WISPY_MACOS)
    json_object *js_jump_key = json_object_object_get(root, "jump_key");
    cfg->jump_key = (int)json_object_get_int(js_jump_key);
    json_object_put(js_jump_key);

    json_object *js_left_key = json_object_object_get(root, "left_key");
    cfg->left_key = (int)json_object_get_int(js_left_key);
    json_object_put(js_left_key);

    json_object *js_right_key = json_object_object_get(root, "right_key");
    cfg->right_key = (int)json_object_get_int(js_right_key);
    json_object_put(js_right_key);

    json_object *js_inventory_key =
        json_object_object_get(root, "inventory_key");
    cfg->inventory_key = (int)json_object_get_int(js_inventory_key);
    json_object_put(js_inventory_key);

#elif defined(WISPY_ANDROID)
    json_object *js_joystick_object = json_object_object_get(root, "joystick");
    cfg->joystick_position = json_object_get_vec2(js_joystick_object);
    json_object_put(js_joystick_object);

    json_object *js_break_object = json_object_object_get(root, "break");
    cfg->break_position = json_object_get_vec2(js_break_object);
    json_object_put(js_break_object);

    json_object *js_place_object = json_object_object_get(root, "place");
    cfg->place_position = json_object_get_vec2(js_place_object);
    json_object_put(js_place_object);

    json_object *js_jump_object = json_object_object_get(root, "jump");
    cfg->jump_position = json_object_get_vec2(js_jump_object);
    json_object_put(js_jump_object);

    json_object *js_inventory_object =
        json_object_object_get(root, "inventory");
    cfg->inventory_position = json_object_get_vec2(js_inventory_object);
    json_object_put(js_inventory_object);
#endif

    json_object_get(root);
    json_object_put(root);
    UnloadFileText(config_data);
  } else {
    memset(cfg, 0, sizeof(w_config));
    cfg->fullscreen = 1;
    cfg->msaa4x = 1;

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
    cfg->jump_key = KEY_SPACE;
    cfg->left_key = KEY_LEFT;
    cfg->right_key = KEY_RIGHT;
    cfg->inventory_key = KEY_LEFT_ALT;
#elif defined(WISPY_ANDROID)
    cfg->joystick_position = VEC(PERCENT_W(0.25), PERCENT_H(0.95));
    cfg->break_position = VEC(RENDER_W - PERCENT_W(0.05), PERCENT_H(0.75));
    cfg->jump_position = VEC(PERCENT_W(0.85), RENDER_H - PERCENT_H(0.05));

#endif

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

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX) || defined(WISPY_MACOS)
  json_object *js_jump_key = json_object_new_int(config->jump_key);
  json_object_object_add(root, "jump_key", js_jump_key);

  json_object *js_left_key = json_object_new_int(config->left_key);
  json_object_object_add(root, "left_key", js_left_key);

  json_object *js_right_key = json_object_new_int(config->right_key);
  json_object_object_add(root, "right_key", js_right_key);

  json_object *js_inventory_key = json_object_new_int(config->inventory_key);
  json_object_object_add(root, "inventory_key", js_inventory_key);

#elif defined(WISPY_ANDROID)
  json_object *js_joystick_object =
      json_object_new_vec2(config->joystick_position);
  json_object_object_add(root, "joystick", js_joystick_object);

  json_object *js_break_object = json_object_new_vec2(config->break_position);
  json_object_object_add(root, "break", js_break_object);

  json_object *js_place_object = json_object_new_vec2(config->place_position);
  json_object_object_add(root, "place", js_place_object);

  json_object *js_jump_object = json_object_new_vec2(config->jump_position);
  json_object_object_add(root, "jump", js_jump_object);

  json_object *js_inventory_object =
      json_object_new_vec2(config->inventory_position);
  json_object_object_add(root, "inventory", js_inventory_object);
#endif

  const char *data =
      json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);
  SaveFileText(config_path, (char *)data);

  json_object_put(root);
  free(config_path);
}

Vector2 json_object_get_vec2(json_object *obj) {
  Vector2 vec = {0};
  json_object *obj_x = json_object_object_get(obj, "x");
  vec.x = (float)json_object_get_double(obj_x);
  json_object_put(obj_x);

  json_object *obj_y = json_object_object_get(obj, "y");
  vec.y = (float)json_object_get_double(obj_y);
  json_object_put(obj_y);

  return vec;
}

json_object *json_object_new_vec2(Vector2 vec) {
  json_object *obj = json_object_new_object();
  json_object *obj_x = json_object_new_double(vec.x);
  json_object_object_add(obj, "x", obj_x);
  json_object *obj_y = json_object_new_double(vec.y);
  json_object_object_add(obj, "y", obj_y);
  return obj;
}