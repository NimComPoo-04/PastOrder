#include <stdio.h>
#include <math.h>
#include <raylib.h>

#include "../util.h"
#include "../map.h"

vertex_t points[MAX_SECTORS * 20] = {0};
int points_count = 0;

float multipler = 40;

line_t walls[MAX_SECTORS * 10] = {0};
int portals[MAX_SECTORS * 10] = {0};
int walls_count = 0;

struct sect_t { int start, end; } sectors[MAX_SECTORS] = {0};
int sector_count = 0;

float player_x = 0;
float player_y = 0;
int player_sector = 0;

int get_neighbour(int s, int j)
{
	line_t tk = walls[j];
	for(int i = 0; i < sector_count; i++)
	{
		if(i == s)
			continue;

		for(int k = sectors[i].start; k <= sectors[i].end; k++)
		{
			if(walls[k].start == tk.start && walls[k].end == tk.end)
				return i;

			if(walls[k].end == tk.start && walls[k].start == tk.end)
				return i;
		}
	}

	return -1;
}

void dump_data()
{
	printf("# Data Generated using builder \n");

	int wid = GetScreenWidth();

	for(int i = 0; i < points_count; i++)
		printf("v\t%.3f %.3f\t%i\n", multipler * points[i].pos.x * 2/wid - 1, multipler * points[i].pos.y * 2/wid - 1, i);

	puts("");

	for(int i = 0; i < sector_count; i++)
	{
		printf("s\t%d\n", i);
		printf("e\t-0.25\n");
		printf("h\t0.25\n");

		if(player_sector == i)
			printf("P\t%.3f %.3f\n", multipler * player_x * 2/wid - 1, multipler * player_y * 2/wid - 1);

		for(int j = sectors[i].start; j <= sectors[i].end; j++)
		{
			if(portals[j])
			{
				int neigh = get_neighbour(i, j);

				printf("p\t%d %d\t%d\t%d\n", walls[j].start, walls[j].end, neigh, j);
			}
			else
			{
				printf("w\t%d %d\t%x %x %x\t%x %x %x\t%d\n",
						walls[j].start, walls[j].end,
						walls[j].col_start.r, walls[j].col_start.g, walls[j].col_start.b,
						walls[j].col_end.r, walls[j].col_end.g, walls[j].col_end.b, j - sectors[i].start);
			}
		}
	}
}

int main(int argc, char **argv)
{
	if(argc == 2)
		multipler = strtod(argv[1], NULL);

	InitWindow(800, 800, "map builder");

	SetTargetFPS(60);

	Camera2D cam = {
		.offset = {GetScreenWidth()/2, GetScreenHeight()/2},
		.target = {0, 0},
		.rotation = 0,
		.zoom = 5
	};

	static char buffer[256];

	while(!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(BLACK);

		Vector2 mousePos = GetMousePosition();

		Vector2 offpos = GetScreenToWorld2D(mousePos, cam);

		int wid = GetScreenWidth();

		offpos.x = (int)offpos.x;
		offpos.y = (int)offpos.y;
		sprintf(buffer, "(%.4lf, %.4lf)", offpos.x * 2. / wid - 1, offpos.y * 2 / wid - 1);

		DrawLine(mousePos.x, mousePos.y - 10, mousePos.x, mousePos.y + 10, WHITE);
		DrawLine(mousePos.x - 10, mousePos.y, mousePos.x + 10, mousePos.y, WHITE);
		DrawText(buffer, mousePos.x + 10, mousePos.y - 30, 20, WHITE);

		float move = (IsKeyDown(KEY_LEFT_SHIFT) ? 0.1 : 0.5);

		float f = GetMouseWheelMove();
		cam.zoom += f * move;
		if(cam.zoom < 1) cam.zoom = 1;

		if(IsKeyDown(KEY_D)) cam.offset.x += move;
		if(IsKeyDown(KEY_A)) cam.offset.x -= move;
		if(IsKeyDown(KEY_W)) cam.offset.y -= move;
		if(IsKeyDown(KEY_S)) cam.offset.y += move;

		cam.target = cam.offset;

		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			points[points_count++].pos = offpos;
		}
		if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{	
			static int wall_started = 0;
			static line_t wall = {0};
			int sel = -1;

			for(int i = 0; i < points_count; i++)
			{
				if((int)offpos.x == (int)points[i].pos.x &&
						(int)offpos.y == (int)points[i].pos.y)
				{
					sel = i;
					break;
				}
			}

			if(sel != -1)
			{
				if(wall_started)
				{
					wall_started = 0;
					wall.end = sel;
					wall.col_start = LIME;
					wall.col_end = YELLOW;

					walls[walls_count++] = wall;
				}
				else
				{
					if(IsKeyDown(KEY_LEFT_SHIFT))
						portals[walls_count] = 1;

					wall_started = 1;
					wall.start = sel;
				}
			}

		}
		if(IsKeyPressed(KEY_Y))
		{
			TraceLog(LOG_INFO, "Sector recorded");
			if(walls_count != 0)
			{
				sectors[sector_count].end = walls_count - 1;
				sector_count++;
				sectors[sector_count].start = walls_count;
			}
		}
		if(IsKeyPressed(KEY_U))
		{
			dump_data();
		}
		if(IsKeyPressed(KEY_I))
		{
			if(sector_count > 0 && points_count > 0)
			{
				player_x = points[points_count-1].pos.x;
				player_y = points[points_count-1].pos.y;
				player_sector = sector_count - 1;
				points_count--;
			}
		}


		BeginMode2D(cam);

		// Grid Drawing
		for(int i = 0; i < wid; i += 10)
		{
			DrawLine(i, 0, i, wid, GRAY);
			DrawLine(0, i, wid, i, GRAY);
		}

		DrawCircleV(offpos, 0.5, RED);
		DrawCircle(player_x, player_y, 0.5, GREEN);

		for(int i = 0; i < points_count; i++)
			DrawCircleV(points[i].pos, 0.5, PURPLE);

		for(int i = 0; i < walls_count; i++)
			DrawLineV(points[walls[i].start].pos, points[walls[i].end].pos, portals[i] ? SKYBLUE : YELLOW);

		EndMode2D();

		DrawFPS(5, 5);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
