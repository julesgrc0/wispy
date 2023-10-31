#include "player.h"

void update_player(Player *player, float dt)
{
	if (IsKeyDown(KEY_RIGHT) && player->velocity.x < (PLAYER_VELOCITY / 2))
	{
		player->velocity.x = PLAYER_VELOCITY;
		player->direction = 0;
	}
	else if (IsKeyDown(KEY_LEFT) && player->velocity.x > -(PLAYER_VELOCITY / 2))
	{
		player->velocity.x = -PLAYER_VELOCITY;
		player->direction = 1;
	}

	if (player->delay > 0)
	{
		player->delay -= dt;
	}
	else
	{
		if (IsKeyDown(KEY_SPACE) && player->onground == 1)
		{
			player->velocity.y = -(PLAYER_VELOCITY * 20);
			player->delay = 0.3f;
			player->onground = 0;
		}
		else if (player->velocity.y >= 0 && player->onground == 0)
		{
			player->velocity.y = PLAYER_VELOCITY * 5;
		}
	}
}
