#include "loading.h"

void load_assets(w_state *state) {
  state->render = LoadRenderTexture(RENDER_W, RENDER_H);
  // LoadRenderTexture(state->config->render_size, state->config->render_size);
  state->src_rnd = (Rectangle){0.0f, 0.0f, (float)state->render.texture.width,
                               -(float)state->render.texture.height};
  state->dest_rnd = (Rectangle){0.0f, 0.0f, (float)GetScreenWidth(),
                                (float)GetScreenHeight()};

  size_t len;
  w_asset *items = unpack_assets(state->hInstance, &len);
  if (!items) {
    state->state = F_FAILED;
    return;
  }

  state->textures = malloc(sizeof(Texture) * len);
  state->textures_id = malloc(sizeof(char *) * len);

  state->font = GetFontDefault();

  size_t textures_index = 0;
  for (size_t i = 0; i < len; i++) {
    const char *ext = strrchr(items[i].name, '.');

    if (strcmp(ext, ".png") == 0) {
      Image image = LoadImageFromMemory(".png", items[i].buffer, items[i].size);
      state->textures[textures_index] = LoadTextureFromImage(image);
      UnloadImage(image);

      state->textures_id[textures_index] = items[i].name;
      textures_index++;
    } else if (strcmp(ext, ".ttf") == 0) {
      int fontSize = 72;
      char fontChars[73] =
          "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()?:+-"
          "=*\"'";
      state->font = LoadFontFromMemory(".ttf", items[i].buffer, items[i].size,
                                       fontSize, fontChars, 73);

      sfree(items[i].name);
    } else {
      sfree(items[i].name);
      sfree(items[i].buffer);
      continue;
    }

    sfree(items[i].buffer);
  }

  state->len = textures_index;
  state->textures = realloc(state->textures, sizeof(Texture) * state->len);
  state->textures_id = realloc(state->textures_id, sizeof(char *) * state->len);

  sfree(items);

  state->state = F_OK;
}

void loading_screen(w_state *state) {
  bool loaded = false;

  int text_size = 50;

  const char *loading_text = "Loading...";
  const char *error_text = "Failed to load resources !";

  Vector2 loading_pos = {
      .x = (GetScreenWidth() - MeasureText(loading_text, text_size)) / 2.f,
      .y = (GetScreenHeight() - text_size) / 2.f};

  Vector2 error_pos = {
      .x = (GetScreenWidth() - MeasureText(error_text, text_size)) / 2.f,
      .y = (GetScreenHeight() - text_size) / 2.f};

  while (!loaded && !WindowShouldClose()) {
    switch (state->state) {
    case F_DISPLAY: {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText(loading_text, loading_pos.x, loading_pos.y, text_size, WHITE);
      EndDrawing();
      state->state = F_LOAD;
    } break;
    case F_LOAD:
      load_assets(state);
      break;
    case F_FAILED: {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText(error_text, error_pos.x, error_pos.y, text_size, RED);
      EndDrawing();
    } break;
    case F_OK:
      loaded = true;
      break;
    }
  }
}
