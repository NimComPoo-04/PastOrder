#include <raylib.h>
#include <raymath.h>

#include "map.h"

const char *name = "demo.map";

int main(int argc, char **argv)
{
	if(argc == 2)
		name = argv[1];

	player_t p = {
		.pos = {0, 0},
		.size = 0.1,
		.angle = 0,
		.vel = 1,
		.fov = PI/2,
		.eye = 0.1,
		.sectId = 0
	};

	assets_t as = {0};
	assets_load_image(&as, "assets/beluga.png");

	map_t m = {0};
	m.player = &p;
	m.eye_height = 0.5;
	m.crouch_height = 0.25;
	m.assets = &as;

	map_load_data(&m, name);
	map_dump(&m);	

	InitWindow(800, 800, "...");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(DARKBROWN);

		map_draw_sectors(&m);

		DrawFPS(0, 0);

		EndDrawing();

		player_update(&p, &m);
		map_update(&m);
	}

	CloseWindow();

	assets_unload(&as);

	return 0;
}
