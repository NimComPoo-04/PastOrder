#ifndef _MAP_H_
#define _MAP_H_

#define MAX_SECTORS 100

#include <raylib.h>
#include <raymath.h>

#include "entity.h"

// Vertex information
typedef struct vertex_t
{
	Vector2 pos;
} vertex_t;

// Lines
typedef struct line_t
{
	int start;
	int end;

	Color   col_start;
	Color   col_end;
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

	entity_t *entities;
	int entities_count;

	vertex_t *vertices;
	int vertex_count;

	sector_t *sectors;
	int sectors_count;

	float eye_height;
	float crouch_height;

	int debug_view;
} map_t;

void map_load_data(map_t *m, const char *file);
void map_draw_vertices(map_t *m);	// render all the verteces
void map_draw_walls(map_t *m);		// render all the sector walls
void map_draw_sectors(map_t *m);	// render all the vsible sectors starting from the current sector
void map_dump(map_t *m);

#endif
