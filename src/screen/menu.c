#include "menu.h"

void menu_screen(w_state *state) {
  ShowCursor();

  bool is_active = true;
  Shader blurShader = get_shader_by_id(state, "shaders\\menu_blur.fs");

  Texture block_textures[6] = {
      get_texture_by_id(state, "blocks\\grass.png"),
      get_texture_by_id(state, "blocks\\dirt.png"),
      get_texture_by_id(state, "blocks\\stone.png"),
      get_texture_by_id(state, "blocks\\mineral.png"),
      get_texture_by_id(state, "blocks\\mineral_or.png"),
      get_texture_by_id(state, "blocks\\mineral_iron.png")};

  w_chunkgroup *grp = create_chunkgroup(CHUNK_GROUP_MID_LEN);
  w_chunkview *view = create_chunkview(grp->chunks[0]);

  Camera2D camera = {0};
  camera.zoom = 1.0f;
  camera.target = (Vector2){.x = CHUNK_GROUP_MID_LEN * CHUNK_W * CUBE_W,
                            .y = CHUNK_MID_H * CUBE_H};

  w_guicontext *ctx =
      create_gui((Vector2){(float)GetRenderWidth(), (float)GetRenderHeight()});
  ctx->font_size = 25;
  ctx->margin_height = 10;
  ctx->margin_width = 40;

  w_guibutton *play_button = create_button(
      ctx, percent_to_pixel(ctx, (Vector2){0.5f, 0.45f}), WHITE, "Play");
  w_guibutton *setting_button =
      create_button(ctx,
                    (Vector2){
                        .x = play_button->position.x + play_button->size.x / 2,
                        .y = play_button->rect.y + play_button->rect.height * 2,
                    },
                    WHITE, "Settings");
  setting_button->default_color = Fade(WHITE, 0.5);
  setting_button->hover_color = Fade(WHITE, 0.5);

  w_guibutton *exit_button = create_button(
      ctx,
      (Vector2){
          .x = play_button->position.x + play_button->size.x / 2,
          .y = setting_button->rect.y + setting_button->rect.height * 2,
      },
      WHITE, "Exit");

  w_guitext *title_text = create_text(
      ctx, percent_to_pixel(ctx, (Vector2){0.5f, 0.2f}), "Wispy", 120, WHITE);
  center_text(title_text, true, true);

  w_guitext *credit_text = create_text(
      ctx,
      Vector2Add(title_text->position,
                 (Vector2){0, title_text->font_size + 10}),
      (char *)TextFormat("made by @julesgrc0 - %s", WISPY_VERSION), 20, WHITE);

  double angle = 0.0;

  while (!WindowShouldClose() && is_active) {

    double speed = GetFrameTime() * 0.1;
    angle += speed;
    angle = fmod(angle, 360.0);

    camera.target.x += GetFrameTime() * 10.f; //(float)((sin(angle) * 1000.0) * speed);
    camera.target.y += (float)((cos(angle) * 1000.0) * speed);

    update_chunkview(view, grp, get_camera_view(&camera));
    update_chunkview_lighting(
        view, Vector2Add(camera.target, (Vector2){RENDER_W / 2, RENDER_H / 2}),
        DEFAULT_LIGHT_RADIUS * 0.75);

    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H, (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});
    BeginMode2D(camera);
    for (unsigned int i = 0; i < view->textures_len; i++) {
      DrawTexturePro(block_textures[view->blocks[i].block.type - 1],
                     view->blocks[i].src, view->blocks[i].dst, VEC_ZERO, 0,
                     view->blocks[i].light);
    }
    EndMode2D();

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    BeginShaderMode(blurShader);
    DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd,
                   VEC_ZERO, 0.0f, WHITE);
    EndShaderMode();

    DrawRectangleLinesEx(
        (Rectangle){0, 0, ctx->render_size.x, ctx->render_size.y}, 5,
        Fade(BLACK, 0.9f));

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

  if (is_active) {
    state->state = FS_EXIT;
  }
}
