#include "menu.h"

void menu_screen(w_state *state) {
#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
  ShowCursor();
#endif

  bool is_active = true;

  Texture block_textures[3] = {get_texture_by_id(state, "blocks\\grass.png"),
                               get_texture_by_id(state, "blocks\\dirt.png"),
                               get_texture_by_id(state, "blocks\\stone.png")};

  w_guicontext *ctx = create_gui();
  ctx->font_size = 25;
  ctx->margin_height = 10;
  ctx->margin_width = 40;

  w_guibutton *play_button =
      create_button(ctx, VEC(PERCENT_W(0.5f), PERCENT_H(0.45f)), WHITE, "Play");
  w_guibutton *setting_button = create_button(
      ctx,
      VEC(.x = play_button->position.x + play_button->size.x / 2,
          .y = play_button->rect.y + play_button->rect.height * 2, ),
      WHITE, "Settings");
  setting_button->default_color = Fade(WHITE, 0.5);
  setting_button->hover_color = Fade(WHITE, 0.5);

  w_guibutton *exit_button = create_button(
      ctx,
      VEC(.x = play_button->position.x + play_button->size.x / 2,
          .y = setting_button->rect.y + setting_button->rect.height * 2),
      WHITE, "Exit");

  w_guitext *title_text = create_text(
      ctx, VEC(PERCENT_W(0.5f), PERCENT_H(0.2f)), "Wispy", 120, WHITE);
  center_text(title_text, true, true);

  w_guitext *credit_text = create_text(
      ctx, Vector2Add(title_text->position, VEC(0, title_text->font_size + 10)),
      (char *)TextFormat("made by @julesgrc0 - %s", WISPY_VERSION), 20, WHITE);

  
  
  /// TODO: pre-render to a texture
  unsigned int cubes[RENDER_CUBE_COUNT * RENDER_CUBE_COUNT] = {0};
  for (unsigned int y = 0; y < RENDER_CUBE_COUNT; y++) {
    for (unsigned int x = 0; x < RENDER_CUBE_COUNT; x++) {
      /// TODO: fix sin wave
      cubes[x + y * RENDER_CUBE_COUNT] = (int)(sin(x / 100.0f) * 1.5f + 1.5f);
    }
  }


  while (!WindowShouldClose() && is_active) {

    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H, (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});

    for (unsigned int y = 0; y < RENDER_CUBE_COUNT; y++) {
      for (unsigned int x = 0; x < RENDER_CUBE_COUNT; x++) {
        DrawTexturePro(
            block_textures[
              cubes[x + y * RENDER_CUBE_COUNT]
            ], CUBE_SRC_RECT,
            RECT((float)x * CUBE_W, (float)y * CUBE_H, CUBE_W, CUBE_H),
            VEC_ZERO, 0, WHITE);
      }
    }

    /// TODO: move to home view (start)
    if (update_button(play_button)) {
      is_active = false;
    }
    if (update_button(setting_button)) {
      /// TODO: Implement settings screen
    }
    if (update_button(exit_button)) {
      state->state = FS_EXIT;
      break;
    }
    
    update_text(title_text);
    update_text(credit_text);

    EndTextureMode();


    BeginDrawing();
    ClearBackground(BLACK);
    draw_render_texture(state);
    EndDrawing();
  }

  destroy_text(credit_text);
  destroy_text(title_text);
  destroy_button(play_button);
  destroy_button(setting_button);
  destroy_button(exit_button);
  destroy_gui(ctx);

  if (is_active) {
    state->state = FS_EXIT;
  }
}
