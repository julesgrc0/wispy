#include "loading.h"

void load_assets(w_state *state) {
  state->render = LoadRenderTexture(RENDER_W, RENDER_H);
  // LoadRenderTexture(state->config->render_size, state->config->render_size);
  state->src_rnd = (Rectangle){0.0f, 0.0f, (float)state->render.texture.width,
                               -(float)state->render.texture.height};
  state->dest_rnd = (Rectangle){0.0f, 0.0f, (float)GetScreenWidth(),
                                (float)GetScreenHeight()};

  size_t items_len;
  #ifdef _WIN32
  w_asset *items = unpack_assets(state->hInstance, &items_len);
  #else
  w_asset *items = unpack_assets(&items_len);
  #endif
  if (!items) {
    state->state = FS_FAILED;
    return;
  }

  state->textures = malloc(sizeof(Texture) * items_len);
  state->textures_id = malloc(sizeof(char *) * items_len);

  state->shaders = malloc(sizeof(Texture) * items_len);
  state->shaders_id = malloc(sizeof(char *) * items_len);

  state->font = GetFontDefault();

  for (size_t i = 0; i < items_len; i++) {
    const char *ext = strrchr(items[i].name, '.');

    if (strcmp(ext, ".png") == 0) {
      Image image = LoadImageFromMemory(".png", items[i].buffer, items[i].size);
      state->textures[state->textures_len] = LoadTextureFromImage(image);
      SetTextureWrap(state->textures[state->textures_len], TEXTURE_WRAP_CLAMP);
      UnloadImage(image);

      state->textures_id[state->textures_len] = items[i].name;
      state->textures_len++;
    } else if (strcmp(ext, ".ttf") == 0) {
      int fontSize = 72;
      char fontChars[73] =
          "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()?:+-"
          "=*\"'";
      state->font = LoadFontFromMemory(".ttf", items[i].buffer, items[i].size,
                                       fontSize, fontChars, 73);

      sfree(items[i].name);
    } else if (strcmp(ext, ".vs") == 0 || strcmp(ext, ".fs") == 0) {
      bool is_vertex = strcmp(ext, ".vs") == 0;

      items[i].buffer = realloc(items[i].buffer, items[i].size + 1);
      items[i].buffer[items[i].size] = '\0';

      state->shaders[state->shaders_len] =
          LoadShaderFromMemory(is_vertex ? items[i].buffer : NULL,
                               is_vertex ? NULL : items[i].buffer);
      state->shaders_id[state->shaders_len] = items[i].name;
      state->shaders_len++;

    } else {
      sfree(items[i].name);
      sfree(items[i].buffer);
      continue;
    }

    sfree(items[i].buffer);
  }

  state->textures =
      realloc(state->textures, sizeof(Texture) * state->textures_len);
  state->textures_id =
      realloc(state->textures_id, sizeof(char *) * state->textures_len);

  state->shaders = realloc(state->shaders, sizeof(Shader) * state->shaders_len);
  state->shaders_id =
      realloc(state->shaders_id, sizeof(char *) * state->shaders_len);

  sfree(items);

  state->state = FS_OK;
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
    case FS_DISPLAY: {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText(loading_text, loading_pos.x, loading_pos.y, text_size, WHITE);
      EndDrawing();
      state->state = FS_LOAD;
    } break;
    case FS_LOAD:
      load_assets(state);
      break;
    case FS_FAILED: {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText(error_text, error_pos.x, error_pos.y, text_size, RED);
      EndDrawing();
    } break;
    case FS_OK:
      loaded = true;
      break;
    }
  }
}
