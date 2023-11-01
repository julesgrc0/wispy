#include "game.h"

#define render_chunk(chunk, position, box)                                                                                                   \
	for (size_t i = 0; i < chunk->len; i++)                                                                                                  \
	{                                                                                                                                        \
		if (chunk->blocks[i].x >= box.max.x)                                                                                                 \
			break;                                                                                                                           \
		if (chunk->blocks[i].x < box.min.x)                                                                                                  \
			continue;                                                                                                                        \
		if (chunk->blocks[i].y >= box.max.y)                                                                                                 \
			continue;                                                                                                                        \
		DrawTexturePro(                                                                                                                      \
			blocks[chunk->blocks[i].type - 1],                                                                                               \
			block_rect,                                                                                                                      \
			(Rectangle){                                                                                                                     \
				(chunk->blocks[i].x * cfg->block_size) + ((position)*CHUNK_WIDTH * cfg->block_size), (chunk->blocks[i].y * cfg->block_size), \
				cfg->block_size, cfg->block_size},                                                                                           \
			(Vector2){0}, 0, WHITE);                                                                                                         \
	}

#define round_to(x, to) (round(x/to) * to)

void game_screen(State *state)
{
#ifndef _DEBUG
	HideCursor();
#endif // !_DEBUG

	Config *cfg = state->config;

	Texture blocks[3] = {
		get_texture_by_id(state, "blocks\\grass.png"),
		get_texture_by_id(state, "blocks\\dirt.png"),
		get_texture_by_id(state, "blocks\\stone.png")};

	Texture players[4] = {
		get_texture_by_id(state, "player\\player_idle1.png"),
		get_texture_by_id(state, "player\\player_idle2.png"),
		get_texture_by_id(state, "player\\player_walk1.png"),
		get_texture_by_id(state, "player\\player_walk2.png"),
	};

	Rectangle block_rect = {0, 0, blocks[0].width, blocks[0].height};
	Rectangle player_src = {0, 0, players[0].width, players[0].height};
	Rectangle player_src_rev = {0, 0, -players[0].width, players[0].height};

	BridgeThreadData *bridge = start_thread(state);

	Chunk *chunk = NULL;
	while (!WindowShouldClose())
	{
		BeginTextureMode(state->render);

		ClearBackground(BLACK);
		BeginMode2D(*(bridge->camera));

		chunk = bridge->chunk_current;
		if (chunk != NULL)
		{
			render_chunk(chunk, bridge->position_current, bridge->camera_view_current);

			chunk = bridge->chunk_next;
			if (chunk != NULL)
			{
				render_chunk(chunk, bridge->position_next, bridge->camera_view_next);
			}
		}

		DrawTexturePro(players[bridge->player->state],
					   bridge->player->direction ? player_src_rev : player_src,
					   bridge->player_rect, (Vector2){0}, 0, WHITE);

		/*
		Vector2 mouse = {
			.x = round_to((GetMouseX() * cfg->render_size) / GetRenderWidth(), cfg->block_size) + round_to(bridge->camera->target.x, cfg->block_size),
			.y = round_to((GetMouseY() * cfg->render_size) / GetRenderHeight(), cfg->block_size) + round_to(bridge->camera->target.y, cfg->block_size),
		};
		Vector2 pcenter = {
			.x = bridge->player->position.x + bridge->player_rect.width/2,
			.y = bridge->player->position.y + bridge->player_rect.height/2
		};

		if (Vector2Distance(mouse, pcenter) < cfg->block_size * 5)
		{
			DrawRectangleLines(
				mouse.x,
				mouse.y,
				cfg->block_size,
				cfg->block_size,
				WHITE
			);
		}
		*/

		EndMode2D();
		EndTextureMode();

		BeginDrawing();
		DrawTexturePro(state->render.texture, state->src_rnd, state->dest_rnd, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
		DrawFPS(0, 0);

		EndDrawing();
	}
	stop_thread(bridge);

#ifndef _DEBUG
	ShowCursor();
#endif // !_DEBUG
}
