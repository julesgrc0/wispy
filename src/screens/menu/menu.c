#include "menu.h"

void menu_screen(w_state *state) {
#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
  ShowCursor();
#endif

  bool is_active = true;

  Texture block_textures[6] = {
      get_texture_by_id(state, "blocks\\grass.png"),
      get_texture_by_id(state, "blocks\\dirt.png"),
      get_texture_by_id(state, "blocks\\stone.png"),
      get_texture_by_id(state, "blocks\\mineral.png"),
      get_texture_by_id(state, "blocks\\mineral_or.png"),
      get_texture_by_id(state, "blocks\\mineral_iron.png")};

  w_chunkgroup *grp = create_chunkgroup(CHUNK_GROUP_MID_LEN);
  w_chunkview *view = create_chunkview(grp->chunks[0]);
  w_camera *camera = create_camera(0, CHUNK_MID_H * CUBE_H);

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

  float angle = 0.0;
  while (!WindowShouldClose() && is_active) {

    float speed = GetFrameTime() * 0.1f;
    angle += speed;
    angle = fmodf(angle, 360.f);

#if defined(WISPY_ANDROID)
    camera->position.x += sinf(angle) * 1000.f * speed;
    camera->position.y += cosf(angle) * 1000.f * speed;
#else
    add_camera_vec(camera, VEC(sinf(angle) * 1000.f * speed,
                               cosf(angle) * 1000.f * speed));
#endif

    update_chunkview(view, grp, camera);
    update_chunkview_lighting(view, get_camera_center(camera),
                              DEFAULT_LIGHT_RADIUS * 0.75);

    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H, (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
    begin_camera(camera);
#endif

    for (unsigned int i = 0; i < view->len; i++) {
      DrawTexturePro(block_textures[view->blocks[i].block.type - 1],
                     view->blocks[i].src,
#if defined(WISPY_ANDROID)
                     get_rectangle_camera(view->blocks[i].dst, camera),
#else
                     view->blocks[i].dst,
#endif
                     VEC_ZERO, 0, view->blocks[i].light);
    }

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
    end_camera();
#endif

    if (update_button(play_button)) {
      is_active = false;
    }
    if (update_button(setting_button)) {
      // TODO: Implement settings screen
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

  destroy_chunkgroup(grp);
  destroy_chunkview(view);

  destroy_text(credit_text);
  destroy_text(title_text);
  destroy_button(play_button);
  destroy_button(setting_button);
  destroy_button(exit_button);
  destroy_gui(ctx);

  destroy_camera(camera);

  if (is_active) {
    state->state = FS_EXIT;
  }
}
