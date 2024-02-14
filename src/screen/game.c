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
  w_blockbreaker *bb = create_blockbreaker(state, td->chunk_view, td->camera);
  while (!WindowShouldClose() && td->is_active) {
    update_keyboard(td->keyboard);

    float dt = GetFrameTime();
    float speed = dt * PLAYER_SPEED;

#ifdef _WIN32
    WaitForSingleObject(td->chunk_view->mutex, INFINITE);
#else
    pthread_mutex_lock(&td->chunk_view->mutex);
#endif // _WIN32
    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H, (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});

    BeginMode2D(*(td->camera));
    smooth_vec(&td->camera->target, td->camera_target, speed);

    for (unsigned int i = 0; i < td->chunk_view->textures_len; i++) {
      DrawTexturePro(block_textures[td->chunk_view->blocks[i].block.type - 1],
                     td->chunk_view->blocks[i].src,
                     td->chunk_view->blocks[i].dst, VEC_ZERO, 0,
                     td->chunk_view->blocks[i].light);
    }
    w_breakstate bstate = update_blockbreaker(bb, td->player, dt);

    if (bstate == BS_BREAKING) {
      draw_blockbreaker(bb);
    } else if (bstate == BS_BROKEN) {
      td->force_update = true;
    }

    DrawTexturePro(player_textures[td->player->state], td->player->src,
                   (Rectangle){.x = td->player->position.x,
                               .y = td->player->position.y,
                               .width = td->player->dst.width,
                               .height = td->player->dst.height},
                   VEC_ZERO, 0, WHITE);

    EndMode2D();
    EndTextureMode();
#ifdef _WIN32
    ReleaseMutex(td->chunk_view->mutex);
#else
    pthread_mutex_unlock(&td->chunk_view->mutex);
#endif // _WIN32

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd,
                   VEC_ZERO, 0.0f, WHITE);
    DrawFPS(0, 0);
    EndDrawing();
  }
  destroy_blockbreaker(bb);
  destroy_bridge(td);
}
