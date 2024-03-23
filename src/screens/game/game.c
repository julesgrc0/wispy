#include "game.h"

void game_screen(w_state *state) {

  Texture block_textures[6] = {
      get_texture_by_id(state, "blocks\\grass.png"),
      get_texture_by_id(state, "blocks\\dirt.png"),
      get_texture_by_id(state, "blocks\\stone.png"),
      get_texture_by_id(state, "blocks\\mineral.png"),
      get_texture_by_id(state, "blocks\\mineral_or.png"),
      get_texture_by_id(state, "blocks\\mineral_iron.png")};

  Texture player_textures[4] = {
      get_texture_by_id(state, "player\\player_idle1.png"),
      get_texture_by_id(state, "player\\player_idle2.png"),
      get_texture_by_id(state, "player\\player_walk1.png"),
      get_texture_by_id(state, "player\\player_walk2.png"),
  };

  w_bridge *td = create_bridge(state->config);
  if (td == NULL)
    return;

  w_blockbreaker *bb = create_blockbreaker(state, td->terrain->view, td->camera);
  if (bb == NULL) {
    destroy_bridge(td);
    return;
  }

#if defined(WISPY_ANDROID)
  Texture break_icon = get_texture_by_id(state, "blocks\\break_icon.png");

  w_guicontext *ctx = create_gui();
  if (ctx == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);
    return;
  }

  w_guijoystick *js = create_joystick(
      ctx, state->config->joysitck_position, PERCENT_W(0.1));
  if (js == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);
    destroy_gui(ctx);
    return;
  }

  w_guiiconbutton *break_button = create_iconbutton(ctx, cfg->break_position,
                                            PERCENT_W(0.05), break_icon);
  if (break_button == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);

    destroy_joystick(js);
    destroy_gui(ctx);
    return;
  }

  w_guiiconbutton *jump_button = create_iconbutton(ctx, cfg->jump_position,
                                           PERCENT_W(0.05), player_textures[3]);
  if (jump_button == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);

    destroy_iconbutton(break_button);
    destroy_joystick(js);
    destroy_gui(ctx);
    return;
  }
#else
    float dt = 0.f;
#endif
  w_breakstate bstate = BS_NONE;

  while (!WindowShouldClose() && td->is_active) {
#if defined(WISPY_ANDROID)
    update_controls(td->ctrl);
    bstate = update_blockbreaker(bb, td->ctrl, td->player, PHYSICS_TICK);
    update_bridge(td);
    smooth_camera(td->camera, PHYSICS_TICK * PLAYER_SPEED * 4.f);
#else
    update_controls(td->ctrl);
    dt = GetFrameTime();
    if (dt <= MIN_FRAME_TIME) {
      set_camera_vec(td->camera, td->camera->target_position);
    } else {
      smooth_camera(td->camera, dt * PLAYER_SPEED);
    }
#endif

#if defined(WISPY_WINDOWS)
    if (TryEnterCriticalSection(&td->terrain->view->csec))
#elif defined(WISPY_LINUX)
    if (pthread_mutex_trylock(&td->terrain->view->mutex) == 0)
#endif
    {

      BeginTextureMode(state->render);
      ClearBackground(BLACK);
      DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H,
                             (Color){66, 135, 245, 255},
                             (Color){142, 184, 250, 255});

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
      begin_camera(td->camera);
#endif

      for (unsigned int i = 0; i < td->terrain->view->len; i++) {
        DrawTexturePro(
            block_textures[td->terrain->view->blocks[i].block.type - 1],
            td->terrain->view->blocks[i].src,
#if defined(WISPY_ANDROID)
            get_rect_to_camera(td->terrain->view->blocks[i].dst, td->camera),
#else
            td->terrain->view->blocks[i].dst,
#endif
            VEC_ZERO, 0, td->terrain->view->blocks[i].light);
      }
#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
      bstate = update_blockbreaker(bb, td->ctrl, td->player, dt);
#endif
      if (bstate == BS_BREAKING) {
        draw_blockbreaker(bb, td->camera);
      } else if (bstate == BS_BROKEN) {
        td->force_update = true;
      }

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
      end_camera();
#endif
      DrawTexturePro(player_textures[td->player->state], td->player->src,
                     RECT((RENDER_W - td->player->dst.width) / 2,
                          (RENDER_H - td->player->dst.height) / 2,
                          td->player->dst.width, td->player->dst.height),
                     VEC_ZERO, 0, WHITE);
#if defined(WISPY_ANDROID)
      td->ctrl->joystick = update_joystick(js);
      td->ctrl->is_breaking = update_iconbutton(break_button);
      td->ctrl->is_jumping = update_iconbutton(jump_button);
#endif
      DrawText(TextFormat("FPS: %i", GetFPS()), 50, 50, 30, WHITE);
      EndTextureMode();
#if defined(WISPY_WINDOWS)
      LeaveCriticalSection(&td->terrain->view->csec);
#elif defined(WISPY_LINUX)
      pthread_mutex_unlock(&td->terrain->view->mutex);
#endif
    }

    BeginDrawing();
    ClearBackground(BLACK);
    draw_render_texture(state);
    EndDrawing();
  }

#if defined(WISPY_ANDROID)
  destroy_joystick(js);
  destroy_iconbutton(break_button);
  destroy_iconbutton(jump_button);
  destroy_gui(ctx);
#endif

  destroy_blockbreaker(bb);
  destroy_bridge(td);
}
