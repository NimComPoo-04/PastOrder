#ifndef _MAP_H_
#define _MAP_H_

#define MAX_SECTORS 100

#include <raylib.h>
#include <raymath.h>

// Player
typedef struct player_t
{
	Vector2 pos;
	float   size;
	float   angle;
	float   vel;
	float   fov;
	float   eye;		// eye height
	int     sectId;
} player_t;

typedef struct map_t map_t;

void player_update(player_t *p, map_t *m);
void player_draw(player_t *p);

// Vertex information
typedef struct vertex_t
{
	Vector2 pos;
	Color   col;
} vertex_t;

// Lines
typedef struct line_t
{
	int start;
	int end;
} line_t;

// sectors
typedef struct sector_t
{
	float elevation;
	float height;

	line_t *walls;		// solid walls arround you
	int walls_count;

	line_t *portals;	// portals arround you
	int portals_count;

	int *neighbours;	// neighbouring sectors
} sector_t;

// Map that holds all this info
typedef struct map_t
{
	player_t *player;

	vertex_t *vertices;
	int vertex_count;

	sector_t *sectors;
	int sectors_count;

	float eye_height;
	float crouch_height;
} map_t;

void map_load_data(map_t *m, const char *file);

void map_draw_vertices(map_t *m);	// render all the verteces
void map_draw_walls(map_t *m);		// render all the sector walls

void map_draw_sectors(map_t *m);	// render all the vsible sectors starting from the current sector

void map_dump(map_t *m);

#endif
