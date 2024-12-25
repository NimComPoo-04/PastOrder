#include <raylib.h>
#include <raymath.h>

#include "map.h"

player_t player = {
	.pos = {0, 0},
	.angle = 0,
	.vel = 0.5,
	.eyelvl = 1,
	.fov = PI/2,
	.size = 0.05
};

wall_t walls[] = {
	{0, {-0.5, -0.5}, {0.5, -0.5}},
	{0, {-0.5, 0.5}, {0.5, 0.5}},
	{1, {-0.5, -0.5}, {-0.5, 0.5}},
	{0, {0.5, -0.5}, {0.5, 0.5}},
};

int main(void)
{
	InitWindow(800, 800, "Past Order");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
//	SetTargetFPS(60);

	while(!WindowShouldClose())
	{
		float dt = GetFrameTime();
		float da = PI/2;

		Vector2 update = {0, 0};

		if(IsKeyDown(KEY_LEFT))
		{
			player.angle -= da * dt;
			update = Vector2Zero();
		}
		if(IsKeyDown(KEY_RIGHT))
		{
			player.angle += da * dt;
			update = Vector2Zero();
		}
		if(IsKeyDown(KEY_UP))
		{
			update =  (Vector2){
				cos(player.angle - PI/2) * player.vel * dt,
				sin(player.angle - PI/2) * player.vel * dt
			};
		}
		if(IsKeyDown(KEY_DOWN))
		{
			update =  (Vector2){
				-cos(player.angle - PI/2) * player.vel * dt,
				-sin(player.angle - PI/2) * player.vel * dt
			};

		}

		player.pos = Vector2Add(player.pos, update);
		for(int i = 0; i < sizeof(walls)/sizeof(walls[0]); i++)
		{
			if(player_wall_collision(&player, walls+i))
			{
				player.pos = Vector2Subtract(player.pos, update);
				break;
			}
		}

		BeginDrawing();
			ClearBackground(BLACK);
			DrawFPS(0, 0);

			walls_debug_draw(walls, sizeof(walls)/sizeof(wall_t), &player);
			player_debug_draw(&player);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
