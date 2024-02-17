#include "state.h"

w_config *load_config()
{
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

  if (FileExists(config_path))
  {
    unsigned int size = 0;
    char *data = LoadFileData(config_path, &size);
    cfg = (w_config *)memmove(cfg, data, sizeof(w_config));

    sfree(data);
  }
  else
  {
#endif // !__ANDROID__

    cfg->fullscreen = 1;
#ifdef _WIN32
    cfg->vsync = 0;
#else
  cfg->vsync = 1;
#endif
    cfg->msaa4x = 1;

    cfg->max_fps = 0;
    cfg->height = 0;
    cfg->width = 0;

#if defined(_DEBUG) && !defined(__ANDROID__)
    cfg->fullscreen = 0;
    cfg->height = 720;
    cfg->width = 1280;
#endif // _DEBUG

#ifndef __ANDROID__
  }
  free(config_path);
#endif // !__ANDROID__

  return cfg;
}

void save_config(w_config *config)
{
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

Texture get_texture_by_id(w_state *state, char *id)
{
  for (size_t i = 0; i < state->textures_len; i++)
  {
    if (strcmp(state->textures_id[i], id) == 0)
    {
      return state->textures[i];
    }
  }

  return (Texture){0};
}

Shader get_shader_by_id(w_state *state, char *id)
{

  for (size_t i = 0; i < state->shaders_len; i++)
  {
    if (strcmp(state->shaders_id[i], id) == 0)
    {
      return state->shaders[i];
    }
  }

  return (Shader){0};
}
