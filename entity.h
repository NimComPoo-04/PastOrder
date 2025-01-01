#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <raylib.h>

// Player
typedef struct entity_t
{
	Vector2 pos;
	float   size;
	float   view_size;
	float   angle;
	float   vel;
	float   fov;
	float   eye;		// eye height
	int     sectId;
} entity_t;

typedef struct map_t map_t;

// for now this is fine lol
typedef entity_t player_t;

void player_update(player_t *p, map_t *m);
void player_draw(player_t *p);

void entity_update(entity_t *e, map_t *m);
void entity_draw(entity_t *e, map_t *m);


#endif
