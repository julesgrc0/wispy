#include "menu.h"

void menu_screen(w_state *state) {
#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
  ShowCursor();
#endif

  bool is_active = true;

  Texture block_textures[3] = {get_texture_by_id(state, "blocks\\grass.png"),
                               get_texture_by_id(state, "blocks\\dirt.png"),
                               get_texture_by_id(state, "blocks\\stone.png")};

  w_menu_view *home_view = create_menu_home(state);
  w_menu_view *maps_view = create_menu_maps(state);
  w_menu_view *settings_view = create_menu_settings(state);

  w_menu_view *current_view = home_view;
  w_menu_state view_state = MENU_STATE_HOME;

  /// TODO: pre-render to a texture
  unsigned int cubes[RENDER_CUBE_COUNT * RENDER_CUBE_COUNT] = {0};
  for (unsigned int y = 0; y < RENDER_CUBE_COUNT; y++) {
    for (unsigned int x = 0; x < RENDER_CUBE_COUNT; x++) {
      /// TODO: fix sin wave
      cubes[x + y * RENDER_CUBE_COUNT] = (int)(sin(x / 100.0f) * 1.5f + 1.5f);
    }
  }

  while (!WindowShouldClose() && is_active && view_state != MENU_STATE_EXIT) {

    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H, (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});

    for (unsigned int y = 0; y < RENDER_CUBE_COUNT; y++) {
      for (unsigned int x = 0; x < RENDER_CUBE_COUNT; x++) {
        DrawTexturePro(
            block_textures[cubes[x + y * RENDER_CUBE_COUNT]], CUBE_SRC_RECT,
            RECT((float)x * CUBE_W, (float)y * CUBE_H, CUBE_W, CUBE_H),
            VEC_ZERO, 0, WHITE);
      }
    }

    view_state = current_view->update(current_view);
    switch (view_state) {
    case MENU_STATE_HOME:
      current_view = home_view;
      break;
    case MENU_STATE_MAPS:
      current_view = maps_view;
      break;
    case MENU_STATE_SETTINGS:
      current_view = settings_view;
      break;
    case MENU_STATE_PLAY:
      is_active = false;
      break;
    }
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    draw_render_texture(state);
    EndDrawing();
  }

  destroy_menu_home(home_view);
  destroy_menu_maps(maps_view);
  destroy_menu_settings(settings_view);

  if (is_active) {
    state->state = FS_EXIT;
  }
}
