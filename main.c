#include <raylib.h>
#include <raymath.h>

#include "map.h"

player_t player = {
	.pos = {0, 0},
	.angle = 0,
	.vel = 0.5,
	.eyelvl = 0,
	.fov = PI/2,
	.size = 0.05,
	.sectId = 1
};

wall_t walls[] =
{
	{{-0.7, 0.8}, {0.964, 0.873}, RED, RED, 0},
	{{0.964, 0.873}, {0.45, 0.215}, RED, RED, 0},

	{{-0.7, 0.8}, {0.45, 0.215}, RED, RED, 1},

	{{0.45, 0.215}, {0.56, -0.467}, RED, RED, 0},
	{{0.56, -0.467}, {0.106, -0.83}, RED, RED, 0},
	{{0.106, -0.83}, {-0.7, 0.8}, RED, RED, 0},
};

sector_t sect[] = {
	{-0.25, 0.25, 3, {0, 1, 2},    {[2] = 1}},
	{-0.20, 0.20, 4, {2, 3, 4, 5}, {[0] = 0}},
};

int main(void)
{
	InitWindow(800, 800, "Past Order");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
//	SetTargetFPS(60);

	while(!WindowShouldClose())
	{
		player_update(&player, sect, walls);

		BeginDrawing();
			ClearBackground(BLACK);
			DrawFPS(0, 0);

	//		walls_debug_draw(walls, sizeof(walls)/sizeof(wall_t), &player);
		//	sector_debug_draw(sect, sizeof(sect)/sizeof(sect[0]), &player,
		//			walls, sizeof(walls)/sizeof(walls[0]));
		//	player_debug_draw(&player);
		
			sector_draw_3d(sect, sizeof(sect)/sizeof(sect[0]), &player,
					walls, sizeof(walls)/sizeof(walls[0]));


		EndDrawing();
	}

	CloseWindow();

	return 0;
}
