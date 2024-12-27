#ifndef _MAP_H_
#define _MAP_H_

#include <raylib.h>
#include <raymath.h>

typedef struct player_t
{
	Vector2 pos;
	float   angle;
	float   vel;
	float   eyelvl;
	float   fov;		// horizontal fov
	float   size;		// the circular radius the player covers
	int     sectId;		// current sector id
} player_t;

typedef struct wall_t wall_t;
typedef struct sector_t sector_t;

void player_debug_draw(player_t *p);
int player_wall_collision(player_t *p, wall_t *w);

void player_update(player_t *p, sector_t *s, wall_t *w);

// Wall can be treated as a subwall using the interp_start and interp_end
typedef struct wall_t
{
	Vector2 point_start, point_end;
	Color   color_start, color_end;
	int isportal;
} wall_t;

// Draw 2d Walls
void walls_debug_draw(wall_t *walls, int count, player_t *p);

// 3d rendering
void wall_draw_3d(wall_t *wall, player_t *p, float elv, float heh);

// Must be convex
typedef struct sector_t
{
	float elevation;
	float height;

	int walls_count;
	int walls[10];

	int neighbour_sectors[10];
} sector_t;

// Draw sectors
void sector_debug_draw(sector_t *s, int count, player_t *p, wall_t *walls, int walls_count);

void sector_draw_3d(sector_t *s, int count, player_t *p, wall_t *walls, int walls_count);

#endif
