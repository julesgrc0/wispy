#include "state.h"

Texture get_texture_from_id(State* state, char* id)
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