#include "game.h"

void game_screen(w_state *state) {
#ifndef _DEBUG
  HideCursor();
#endif // !_DEBUG
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

  while (!WindowShouldClose() && td->is_active) {
    update_keyboard(td->keyboard);

    float speed = GetFrameTime() * PLAYER_SPEED * 0.95f;

    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    DrawRectangleGradientV(0, 0, RENDER_W, RENDER_H, (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});

#ifdef _WIN32
    WaitForSingleObject(td->chunk_view->mutex, INFINITE);
#else
    pthread_mutex_lock(&td->chunk_view->mutex);
#endif // _WIN32

    BeginMode2D(*(td->camera));
    smooth_vec(&td->camera->target, td->camera_target, speed);

    Vector2 player_center = {
        td->player->position.x + td->player->dst.width / 2.f,
        td->player->position.y + td->player->dst.height / 2.f};
    Vector2 block_center = {0};
    for (unsigned int i = 0; i < td->chunk_view->len; i++) {
      block_center.x = td->chunk_view->blocks[i].dst.x + CUBE_W / 2;
      block_center.y = td->chunk_view->blocks[i].dst.y + CUBE_H / 2;
      DrawTexturePro(block_textures[td->chunk_view->blocks[i].block.type - 1],
                     td->chunk_view->blocks[i].src,
                     td->chunk_view->blocks[i].dst, VEC_ZERO, 0,
                     abs(Vector2Distance(player_center, block_center)) <
                             CUBE_W * 2
                         ? BLUE
                         : WHITE);
    }
    EndMode2D();
    DrawTexturePro(player_textures[td->player->state], td->player->src,
                   td->player->dst, VEC_ZERO, 0, WHITE);
    /*
    DrawText(
        TextFormat(
            "Block %d\n\nCamera %f %f\n\Player %d\n\nChunk:\n   "
            "Current: %d\n   Next: %d\n   Group: %d",
            td->chunk_view->len, td->camera->target.x, td->camera->target.y,
            round(td->player->position.x / FULL_CHUNK_W),
            td->chunk_view->target->position,
            td->chunk_view->next == NULL ? -1 : td->chunk_view->next->position,
            td->chunk_group->position),
        0, 20, 20, WHITE);
      */
#ifdef _WIN32
    ReleaseMutex(td->chunk_view->mutex);
#else
    pthread_mutex_unlock(&td->chunk_view->mutex);
#endif // _WIN32

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd,
                   VEC_ZERO, 0.0f, WHITE);
    DrawFPS(0, 0);
    /*
    DrawText(
        TextFormat("dt: %f\nfps: %f", GetFrameTime(), 1.f / GetFrameTime()), 0,
        0, 20, WHITE);
    */
    EndDrawing();
  }
  destroy_bridge(td);

#ifndef _DEBUG
  ShowCursor();
#endif // !_DEBUG
}
