#include "state.h"

Config* load_config()
{
	Config* cfg = malloc(sizeof(Config));
	if (cfg == NULL) return NULL;

	char* config_path = GetApplicationDirectory();
	strcat(config_path, CONFIG_NAME);

	if (FileExists(config_path))
	{
		unsigned int size = 0;
		char* data = LoadFileData(config_path, &size);
		cfg = (Config*)memmove(cfg, data, sizeof(Config));
		
		sfree(data);

		if (cfg->render_distance == 0)
		{
			cfg->render_distance = 20;
		}
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

		cfg->render_distance = 20;
		cfg->render_size = 500;
		
#ifdef _DEBUG
		cfg->fullscreen = 0;
		cfg->height = 500;
		cfg->width = 500;
#endif // _DEBUG
	}

	float fblock = cfg->render_size / (float)cfg->render_distance;
	cfg->block_size = round(fblock);
	cfg->max_render_block = round((cfg->render_size + fblock) / fblock);
	
	return cfg;
}

void save_config(Config* config)
{
	char* config_path = GetApplicationDirectory();
	strcat(config_path, CONFIG_NAME);
	SaveFileData(config_path, config, sizeof(Config));
}

Texture get_texture_by_id(State* state, char* id)
{
	for (size_t i = 0; i < state->len; i++)
	{
		if (strcmp(state->textures_id[i], id) == 0)
		{
			return state->textures[i];
		}
	}

	return (Texture) { 0 };
}