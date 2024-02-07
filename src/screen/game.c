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

  Vector2 camera_target = center_camera_on_object(td->camera, td->player->box);
  while (!WindowShouldClose() && td->is_active) {
    update_keyboard(td->keyboard);

    if (td->keyboard->key != 0) {
      camera_target = center_camera_on_object(td->camera, td->player->box);
    }

    float speed = GetFrameTime() * 1000.f;

    smooth_camera(td->camera->target.x, camera_target.x, speed);
    smooth_camera(td->camera->target.y, camera_target.y, speed);

    BeginTextureMode(state->render);
    ClearBackground(BLACK);
    BeginMode2D(*(td->camera));

#ifdef _WIN32
    WaitForSingleObject(td->chunk_view->mutex, INFINITE);
#else
    pthread_mutex_lock(&td->chunk_view->mutex);
#endif // _WIN32
    for (unsigned int i = 0; i < td->chunk_view->len; i++) {
      DrawTexturePro(block_textures[td->chunk_view->blocks[i].block.type - 1],
                     td->chunk_view->blocks[i].src,
                     td->chunk_view->blocks[i].dst, VEC_ZERO, 0, WHITE);
    }

    EndMode2D();
    DrawText(TextFormat("Block %d\n\nCamera %f %f\n\Player %f %f\n\nChunk:\n   "
                        "Current: %d\n   Next: %d\n   Group: %d",
                        td->chunk_view->len, td->camera->target.x,
                        td->camera->target.y, td->player->box.x,
                        td->player->box.y, td->chunk_view->target->position,
                        td->chunk_view->next == NULL
                            ? -1
                            : td->chunk_view->next->position,
                        td->chunk_group->position),
             0, 20, 20, WHITE);
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
  destroy_bridge(td);

#ifndef _DEBUG
  ShowCursor();
#endif // !_DEBUG
}
