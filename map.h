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
	float   fov;	// horizontal fov
	float   size;	// the circular radius the player covers
} player_t;

typedef struct wall_t wall_t;

void player_debug_draw(player_t *p);
int player_wall_collision(player_t *p, wall_t *w);

// Wall can be treated as a subwall using the interp_start and interp_end
typedef struct wall_t
{
	int isportal;
	Vector2 point_start, point_end;
	Color   color_start, color_end;
} wall_t;

// Draw 2d Walls
void walls_debug_draw(wall_t *walls, int count, player_t *p);

#endif
