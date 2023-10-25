#include "controller.h"

ControllerThreadData* start_controller(State* state, Camera2D* camera, Player* player)
{
	ControllerThreadData* ctd = malloc(sizeof(ControllerThreadData));
	if (ctd == NULL) return NULL;

	ctd->active = 1;

	ctd->state = state;
	ctd->player = player;
	ctd->camera = camera;

	ctd->chunk_current = NULL;
	ctd->chunk_next = NULL;

	ctd->position_current = 0;
	ctd->position_next = 0;


	ctd->camera->target.x = ctd->player->position.x - ctd->state->config->render_size / 2;
	ctd->camera->target.y = ctd->player->position.y - ctd->state->config->render_size / 2;

#ifdef _WIN32
	ctd->handle = INVALID_HANDLE_VALUE;
	ctd->handle = CreateThread(NULL, NULL, &controller_thread, ctd, 0, NULL);
#endif // _WIN32

	return ctd;
}

void stop_controller(ControllerThreadData* ctd)
{
	ctd->active = 0;

#ifdef _WIN32
	if(ctd->handle != INVALID_HANDLE_VALUE) WaitForSingleObject(ctd->handle, INFINITE);
#endif // _WIN32

	sfree(ctd)
}

DWORD WINAPI controller_thread(LPVOID arg)
{
	ControllerThreadData* ctd = arg;
	Config* cfg = ctd->state->config;

	float dt = 1.f/(float)(cfg->max_fps == 0 ? (1/60.0) : cfg->max_fps);
	float animation = 0.f;

	InitPhysics(dt * 100.);
	

	Camera2D* camera = ctd->camera;

	Player* player = ctd->player;
	PhysicsBody playerBody = CreatePhysicsBodyRectangle((Vector2){
		.x = player->position.x,
		.y = player->position.y - cfg->block_size * 3
	}, cfg->block_size, cfg->block_size * 2, 1);

	playerBody->freezeOrient = true;

	while (ctd->active)
	{
		if (ctd->chunk_current == NULL || (ctd->chunk_current == NULL && ctd->chunk_next == NULL)) continue;

		update_player(player, playerBody);

		animation += dt * .5f;
		if (playerBody->velocity.x != 0)
		{
			player->state = ((int)animation % 2 == 0) ? P_WALK_1 : P_WALK_2;
		}else player->state = ((int)animation % 2 == 0) ? P_IDLE_1 : P_IDLE_2;

		

		Chunk* chunk = ctd->chunk_current;
		unsigned int position = ctd->position_current;

		if (ctd->player->position.x > ((ctd->position_current + 1) * CHUNK_WIDTH * cfg->block_size))
		{
			chunk = ctd->chunk_next;
			position = ctd->position_next;
		}



		unsigned int bodiesCount = GetPhysicsBodiesCount();
		for (unsigned int id = 0; id < bodiesCount; id++)
		{
			PhysicsBody body = GetPhysicsBody(id);
			if (body->id != playerBody->id)
			{
				DestroyPhysicsBody(body);
			}
		}

		Vector2 block = { 0 };
		for (size_t i = 0; i < chunk->len; i++)
		{
			block.x = chunk->blocks[i].x * cfg->block_size + position * CHUNK_WIDTH * cfg->block_size;
			block.y = chunk->blocks[i].y * cfg->block_size;

			if (Vector2Distance(block, player->position) < cfg->block_size * 2)
			{
				PhysicsBody blockBody = CreatePhysicsBodyRectangle(block, cfg->block_size, cfg->block_size, 10);
				blockBody->enabled = false;

			}
		}

		RunPhysicsStep();
		player->position = playerBody->position;
		//smooth_camera(camera->target.x, player->position.x - (cfg->render_size / 2), 200.f);
		//smooth_camera(camera->target.y, player->position.y - (cfg->render_size / 2), 200.f);
	}

	ClosePhysics();

	return 0;
}
