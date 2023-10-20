#include "game.h"


void game_screen(State* state)
{
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);
		ClearBackground(BLACK);

		EndTextureMode();

		BeginDrawing();
		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);

#ifdef _DEBUG
		DrawFPS(0, 0);
#endif // _DEBUG
		EndDrawing();
	}
}
