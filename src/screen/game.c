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

    BeginMode2D(*(td->camera));
    smooth_vec(&td->camera->target, td->camera_target, speed);

#ifdef _WIN32
    WaitForSingleObject(td->chunk_view->mutex, INFINITE);
#else
    pthread_mutex_lock(&td->chunk_view->mutex);
#endif // _WIN32
    Rectangle view = get_camera_view(td->camera);
    DrawRectangleGradientV(view.x, view.y, view.width, view.height,
                           (Color){66, 135, 245, 255},
                           (Color){142, 184, 250, 255});

    for (unsigned int i = 0; i < td->chunk_view->textures_len; i++) {
      DrawTexturePro(block_textures[td->chunk_view->blocks[i].block.type - 1],
                     td->chunk_view->blocks[i].src,
                     td->chunk_view->blocks[i].dst, VEC_ZERO, 0,
                     td->chunk_view->blocks[i].light);
    }
#ifdef _WIN32
    ReleaseMutex(td->chunk_view->mutex);
#else
    pthread_mutex_unlock(&td->chunk_view->mutex);
#endif // _WIN32

#ifdef _DEBUG
    if (td->chunk_view->target != NULL) {
      DrawRectangleLinesEx(
          (Rectangle){.x = td->chunk_view->target->position * FULL_CHUNK_W,
                      .y = 0,
                      .width = FULL_CHUNK_W,
                      .height = FULL_CHUNK_H},
          5, RED);
    }
    if (td->chunk_view->next != NULL) {
      DrawRectangleLinesEx(
          (Rectangle){.x = td->chunk_view->next->position * FULL_CHUNK_W,
                      .y = 0,
                      .width = FULL_CHUNK_W,
                      .height = FULL_CHUNK_H},
          5, GREEN);
    }
#endif

#if 0
    Vector2 mouse =
        Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), *td->camera),
                        (Vector2){(CUBE_W / 2), (CUBE_H / 2)});
    DrawCircleV(GetScreenToWorld2D(GetMousePosition(), *td->camera), 5, RED);

    if (Vector2Distance(mouse, get_player_center(td->player)) < CUBE_W * 3) {
      Rectangle mouse_block = {.x = round(mouse.x / CUBE_W) * CUBE_W,
                               .y = round(mouse.y / CUBE_H) * CUBE_H,
                               .width = CUBE_W,
                               .height = CUBE_H};

      w_block *block = get_chunkview_block(
          td->chunk_view, (Vector2){mouse_block.x, mouse_block.y});
      if (block != NULL && block->type != BLOCK_AIR) {
        DrawRectangleLinesEx(mouse_block, 2, RED);
      }
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      }
    }
#endif
    EndMode2D();
    DrawTexturePro(player_textures[td->player->state], td->player->src,
                   td->player->dst, VEC_ZERO, 0, WHITE);
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd,
                   VEC_ZERO, 0.0f, WHITE);
    DrawFPS(0, 0);
    EndDrawing();
  }
  destroy_bridge(td);

#ifndef _DEBUG
  ShowCursor();
#endif // !_DEBUG
}
