#include "state.h"

w_config *load_config()
{
  w_config *cfg = malloc(sizeof(w_config));
  if (cfg == NULL)
    return NULL;

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

    /*if (cfg->render_distance == 0) {
      cfg->render_distance = 20;
    }*/
  }
  else
  {
    cfg->fullscreen = 1;
    cfg->msaa4x = 1;
    cfg->vsync = 0;
    cfg->max_fps = 0;

    cfg->height = 0;
    cfg->width = 0;

    cfg->max_chunks = 100;

    // cfg->render_distance = 40;
    // cfg->render_size = 1000;

#ifdef _DEBUG
    // cfg->render_distance = 50;
    // cfg->render_size = 1000;

    cfg->fullscreen = 0;
    cfg->height = 720;
    cfg->width = 1280;
#endif // _DEBUG
  }

  /*
  if (cfg->render_distance == 0) cfg->render_distance = 20;

  float fblock = round(cfg->render_size / (float)cfg->render_distance);
  cfg->block_size = fblock;
  cfg->max_render_block = round((cfg->render_size + fblock) / fblock);

  if (cfg->max_render_block == 0) cfg->max_render_block = 255;
  */

  free(config_path);
  return cfg;
}

void save_config(w_config *config)
{
#ifndef _DEBUG
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
  for (size_t i = 0; i < state->len; i++)
  {
    if (strcmp(state->textures_id[i], id) == 0)
    {
      return state->textures[i];
    }
  }

  return (Texture){0};
}