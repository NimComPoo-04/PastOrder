#include <raylib.h>
#include <raymath.h>

#include "map.h"

int main(void)
{
	player_t p = {
		.pos = {0, 0},
		.size = 0.015,
		.angle = 0,
		.vel = 1,
		.fov = PI/2,
		.eye = 0.1,
		.sectId = 0
	};

	map_t m = {0};
	m.player = &p;
	m.eye_height = 0.5;
	m.crouch_height = 0.25;

	map_load_data(&m, "demo.map");
	map_dump(&m);	

	InitWindow(800, 800, "...");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(DARKBROWN);

		//player_draw(&p);

		//map_draw_walls(&m);
		//map_draw_vertices(&m);
		map_draw_sectors(&m);

		DrawFPS(0, 0);

		EndDrawing();

		player_update(&p, &m);
	}

	CloseWindow();

	return 0;
}
