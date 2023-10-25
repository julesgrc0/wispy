#include "player.h"

void update_player(Player* player, PhysicsBody body)
{
	if (IsKeyDown(KEY_LEFT) && body->velocity.x > -(PLAYER_VELOCITY/2))
	{
		body->velocity.x = PLAYER_VELOCITY;
	}
	else if (IsKeyDown(KEY_RIGHT) && body->velocity.x < (PLAYER_VELOCITY/2))
	{
		body->velocity.x = -PLAYER_VELOCITY;
	}

	if (IsKeyPressed(KEY_SPACE) && body->velocity.y >= 0)
	{
		body->velocity.y = -(PLAYER_VELOCITY * 2);
	}
}
