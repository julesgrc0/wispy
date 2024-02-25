#include "state.h"

w_state *create_state() {
  w_state *state = malloc(sizeof(w_state));
  if (state == NULL)
    return NULL;

  memset(state, 0, sizeof(w_state));

  state->state = FS_DISPLAY;
  state->config = load_config();

  return state;
}

void destroy_state(w_state *state) {
  sfree(state->config);
  sfree(state);
}

void draw_render_texture(w_state *state) {
  rlSetTexture(state->render.texture.id);
  rlBegin(RL_QUADS);

  rlColor4ub(255, 255, 255, 255);
  rlNormal3f(0.0f, 0.0f, 1.0f);

  rlTexCoord2f(state->rnd_src.x / state->render.texture.width,
               state->rnd_src.y / state->render.texture.height);
  rlVertex2f(state->rnd_tl.x, state->rnd_tl.y);

  rlTexCoord2f(state->rnd_src.x / state->render.texture.width,
               (state->rnd_src.y + state->rnd_src.height) /
                   state->render.texture.height);
  rlVertex2f(state->rnd_bl.x, state->rnd_bl.y);

  rlTexCoord2f((state->rnd_src.x + state->rnd_src.width) /
                   state->render.texture.width,
               (state->rnd_src.y + state->rnd_src.height) /
                   state->render.texture.height);
  rlVertex2f(state->rnd_br.x, state->rnd_br.y);

  rlTexCoord2f((state->rnd_src.x + state->rnd_src.width) /
                   state->render.texture.width,
               state->rnd_src.y / state->render.texture.height);
  rlVertex2f(state->rnd_tr.x, state->rnd_tr.y);

  rlEnd();
  rlSetTexture(0);
}

Texture get_texture_by_id(w_state *state, char *id) {
  for (size_t i = 0; i < state->textures_len; i++) {
    if (strcmp(state->textures_id[i], id) == 0) {
      return state->textures[i];
    }
  }

  return (Texture){0};
}

Shader get_shader_by_id(w_state *state, char *id) {

  for (size_t i = 0; i < state->shaders_len; i++) {
    if (strcmp(state->shaders_id[i], id) == 0) {
      return state->shaders[i];
    }
  }

  return (Shader){0};
}
