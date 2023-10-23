#pragma once
#include "../stdafx.h"
#include "../core/state.h"
#include "../terrain/chunk.h"

typedef enum PlayerState
{
	P_IDLE_1,
	P_IDLE_2,
	P_WALK_1,
	P_WALK_2
} PlayerState;

#pragma pack(push, 1)
typedef struct Player
{
	Vector2 position;
	unsigned int direction : 1;
	PlayerState state;
} Player;

typedef struct PlayerThreadData
{
	Chunk *chunk_current;
	unsigned int position_current;

	Chunk *chunk_next;
	unsigned int position_next;

	Player *player;
	Camera2D *camera;
	State *state;

	unsigned int active : 1;
#ifdef _WIN32
	HANDLE handle;
#endif // _WIN32

} PlayerThreadData;
#pragma pack(pop)

PlayerThreadData *start_player_thread(Player *player, Camera2D *camera, State *state);
void stop_player_thread(PlayerThreadData *data);

#ifdef _WIN32
DWORD WINAPI player_thread(LPVOID arg);
#endif // _WIN32
