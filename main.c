#include <raylib.h>
#include <raymath.h>

#include "map.h"

const char *name = "demo.map";

entity_t ent[] = {
	{
		.pos = {0, 0},
		.size = 0.02,
		.angle = 0,
		.vel = 1,
		.fov = PI/2,
		.eye = 0.1,
		.sectId = 0
	},
	{
		.pos = {0, 0},
		.size = 0.02,
		.angle = 0,
		.vel = 1,
		.fov = PI/2,
		.eye = 0.1,
		.sectId = 0
	},
	{
		.pos = {0, 0},
		.size = 0.02,
		.angle = 0,
		.vel = 1,
		.fov = PI/2,
		.eye = 0.1,
		.sectId = 0
	},
};

int main(int argc, char **argv)
{
	if(argc == 2)
		name = argv[1];

	player_t p = {
		.pos = {0, 0},
		.size = 0.02,
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

	map_load_data(&m, name);
	map_dump(&m);	

	m.entities = ent;
	m.entities_count = 3;

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
		for(int i = 0; i < (int)(sizeof(ent)/sizeof(ent[0])); i++)
			entity_update(ent + i, &m);
	}

	CloseWindow();

	return 0;
}
