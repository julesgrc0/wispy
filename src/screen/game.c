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

  w_bridge *td = create_bridge();
  if (td == NULL)
    return;

  w_blockbreaker *bb = create_blockbreaker(state, td->chunk_view, td->camera);
  if (bb == NULL) {
    destroy_bridge(td);
    return;
  }

#ifdef __ANDROID__
  w_guicontext *ctx = create_gui();
  if (ctx == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);
    return;
  }

  w_guijoystick *js = create_joystick(
      ctx, VEC(PERCENT_W(0.25), PERCENT_H(0.95)), PERCENT_W(0.1));
  if (js == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);
    destroy_gui(ctx);
    return;
  }

  w_guiaction *break_button = create_action(ctx, VEC(RENDER_W, PERCENT_H(0.8)),
                                            PERCENT_W(0.05), block_textures[0]);
  if (break_button == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);

    destroy_joystick(js);
    destroy_gui(ctx);
    return;
  }

  w_guiaction *jump_button = create_action(ctx, VEC(PERCENT_W(0.95), RENDER_H),
                                           PERCENT_W(0.05), player_textures[3]);
  if (jump_button == NULL) {
    destroy_bridge(td);
    destroy_blockbreaker(bb);

    destroy_action(break_button);
    destroy_joystick(js);
    destroy_gui(ctx);
    return;
  }
#endif // __ANDROID__

  Vector2 target_player = td->player->position;
  w_breakstate bstate = BS_NONE;
  while (!WindowShouldClose() && td->is_active) {
#ifndef __ANDROID__
    update_controls(td->ctrl);
#endif // !__ANDROID__

    float dt = GetFrameTime();
    float speed = dt * PLAYER_SPEED;

#ifdef _WIN32
    if (TryEnterCriticalSection(&td->chunk_view->csec))
#else
    if (pthread_mutex_trylock(&td->chunk_view->mutex) == 0)
#endif // _WIN32
    {

      BeginTextureMode(state->render);
      ClearBackground(BLACK);
      DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H,
                             (Color){66, 135, 245, 255},
                             (Color){142, 184, 250, 255});

      if (dt < MIN_FRAME_TIME) {
        smooth_camera(td->camera, td->camera_target, speed);
        smooth_vec(&target_player, td->player->position,
                   Vector2Distance(td->player->position, target_player) *
                       speed);
      } else {
        set_camera_vec(td->camera, td->camera_target);
        target_player = td->player->position;
      }

      begin_camera(td->camera);
      for (unsigned int i = 0; i < td->chunk_view->len; i++) {
        DrawTexturePro(block_textures[td->chunk_view->blocks[i].block.type - 1],
                       td->chunk_view->blocks[i].src,
                       td->chunk_view->blocks[i].dst, VEC_ZERO, 0,
                       td->chunk_view->blocks[i].light);
      }

#ifndef __ANDROID__
      bstate = update_blockbreaker(bb, td->ctrl, td->player, dt);
#endif // !__ANDROID__

      if (bstate == BS_BREAKING) {
        draw_blockbreaker(bb);
      } else if (bstate == BS_BROKEN) {
        td->force_update = true;
      }

      DrawTexturePro(player_textures[td->player->state], td->player->src,
                     RECT(target_player.x, target_player.y,
                          td->player->dst.width, td->player->dst.height),
                     VEC_ZERO, 0, WHITE);

      end_camera();

#ifdef __ANDROID__
      td->ctrl->joystick = update_joystick(js);
      td->ctrl->is_breaking = update_action(break_button);
      td->ctrl->is_jumping = update_action(jump_button);

      update_controls(td->ctrl);
      bstate = update_blockbreaker(bb, td->ctrl, td->player, dt);
#endif // __ANDROID__
      DrawText(TextFormat("FPS: %i", GetFPS()), 50, 50, 30, WHITE);
      EndTextureMode();

#ifdef _WIN32
      LeaveCriticalSection(&td->chunk_view->csec);
#else
      pthread_mutex_unlock(&td->chunk_view->mutex);
#endif // _WIN32
    }

    BeginDrawing();
    ClearBackground(BLACK);
    draw_render_texture(state);
    EndDrawing();
  }

#ifdef __ANDROID__
  destroy_joystick(js);
  destroy_action(break_button);
  destroy_action(jump_button);
  destroy_gui(ctx);
#endif // __ANDROID__

  destroy_blockbreaker(bb);
  destroy_bridge(td);
}
