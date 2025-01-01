#include "map.h"
#include "entity.h"
#include <raymath.h>

void player_draw(player_t *p)
{
	int wid = GetScreenWidth();
	int hei = GetScreenHeight();

	DrawLine(wid/2, hei/2,
		 wid/2 + 10 * p->size * hei * cos(p->fov/2),
		 hei/2 + 10 * p->size * hei * sin(p->fov/2), SKYBLUE);

	DrawLine(wid/2, hei/2,
		 wid/2 + 10 * p->size * hei * cos(-p->fov/2),
		 hei/2 + 10 * p->size * hei * sin(-p->fov/2), SKYBLUE);

	DrawLine(wid/2, hei/2, wid/2 + p->size * hei + 5, hei/2, SKYBLUE);

	DrawCircle(wid/2, hei/2, 3, MAROON);
	DrawCircleLines(wid/2, hei/2, p->size * hei/2, YELLOW);
}

static int entity_wall_collision(entity_t *e, Vector2 newpos, map_t *m, Vector2 *out)
{
	int flag = 0;

	for(int i = 0; i < m->sectors[e->sectId].walls_count; i++)
	{
		line_t l = m->sectors[e->sectId].walls[i];

		Vector2 a = m->vertices[l.start].pos;
		Vector2 b = m->vertices[l.end].pos;

		Vector2 p = newpos;

		Vector2 n = Vector2Normalize(Vector2Subtract(b, a));
		Vector2 pa = Vector2Subtract(a, p);

		float t = Vector2DotProduct(pa, n);
		Vector2 k = Vector2Scale(n, t);
		Vector2 c = Vector2Subtract(pa, k);

		if(Vector2LengthSqr(c) <= e->size * e->size)
		{
			*out = Vector2Subtract(*out, Vector2Scale(c, e->size));
			flag = 1;
		}
	}

	return flag;
}

void player_update(player_t *p, map_t *m)
{
	float dt = GetFrameTime();

	if(IsKeyDown(KEY_LEFT))
		p->angle -= PI/2 * dt;

	if(IsKeyDown(KEY_RIGHT))
		p->angle += PI/2 * dt;

	Vector2 update = {0};

	if(IsKeyDown(KEY_UP))
		update = (Vector2){cos(p->angle) * p->vel * dt,
			sin(p->angle) * p->vel * dt};

	if(IsKeyDown(KEY_DOWN))
		update = (Vector2){-cos(p->angle) * p->vel * dt,
			-sin(p->angle) * p->vel * dt};


	Vector2 newpos = Vector2Add(p->pos, update);
	Vector2 newupd = {0};

	// TODO: test this extensively so that there are no cases where this fails man
	// idk if this is particularly good way to test things.

	if(entity_wall_collision(p, newpos, m, &newupd))
	{
		newpos = Vector2Add(p->pos, newupd);
	}

	for(int i = 0; i < m->sectors[p->sectId].portals_count; i++)
	{
		line_t l = m->sectors[p->sectId].portals[i];
		int neighbour = m->sectors[p->sectId].neighbours[i];

		Vector2 a = m->vertices[l.start].pos;
		Vector2 b = m->vertices[l.end].pos;

		Vector2 ap = Vector2Subtract(p->pos, a);
		Vector2 an = Vector2Subtract(newpos, a);
		Vector2 ab = Vector2Subtract(b, a);

		float k1 = ap.x * ab.y - ap.y * ab.x;
		float k2 = an.x * ab.y - an.y * ab.x;

		if(k1 * k2 <= 0)
		{
			p->sectId = neighbour;
			break;
		}
	}

	newupd.x = 0;
	newupd.y = 0;

	p->pos = newpos;

	if(IsKeyDown(KEY_LEFT_SHIFT))
		p->eye = m->sectors[p->sectId].elevation + m->crouch_height;
	else
		p->eye = m->sectors[p->sectId].elevation + m->eye_height;
}

void entity_update(entity_t *p, map_t *m)
{
	float dt = GetFrameTime();
	Vector2 update = (Vector2){cos(p->angle) * p->vel * dt, sin(p->angle) * p->vel * dt};

	Vector2 newpos = Vector2Add(p->pos, update);
	Vector2 newupd = {0};

	// TODO: test this extensively so that there are no cases where this fails man
	// idk if this is particularly good way to test things.

	if(entity_wall_collision(p, newpos, m, &newupd))
	{
		newpos = Vector2Add(p->pos, newupd);
		p->angle = GetRandomValue(0, 360) * PI/180;
	}

	for(int i = 0; i < m->sectors[p->sectId].portals_count; i++)
	{
		line_t l = m->sectors[p->sectId].portals[i];
		int neighbour = m->sectors[p->sectId].neighbours[i];

		Vector2 a = m->vertices[l.start].pos;
		Vector2 b = m->vertices[l.end].pos;

		Vector2 ap = Vector2Subtract(p->pos, a);
		Vector2 an = Vector2Subtract(newpos, a);
		Vector2 ab = Vector2Subtract(b, a);

		float k1 = ap.x * ab.y - ap.y * ab.x;
		float k2 = an.x * ab.y - an.y * ab.x;

		if(k1 * k2 <= 0)
		{
			p->sectId = neighbour;
			break;
		}
	}

	newupd.x = 0;
	newupd.y = 0;

	p->pos = newpos;

}

Vector2 map_to_screen(map_t *m, vertex_t v);
void entity_draw(entity_t *e, map_t *m)
{
//	entity_update(e, m);
	Vector2 v = map_to_screen(m, (vertex_t){e->pos});

	Vector2 n = Vector2Normalize(Vector2Subtract(m->player->pos, e->pos));
	Vector2 s1 = {-n.y, n.x};
	Vector2 s2 = {n.y, -n.x};

	Vector2 v1 = Vector2Add(Vector2Scale(s1, e->view_size), e->pos);
	Vector2 v2 = Vector2Add(Vector2Scale(s2, e->view_size), e->pos);

	DrawLineV(map_to_screen(m, (vertex_t){v1}), map_to_screen(m, (vertex_t){v2}), SKYBLUE);

	DrawCircle(v.x, v.y, 3, LIME);
	DrawCircleLines(v.x, v.y, e->size * GetScreenHeight()/2, YELLOW);
	DrawCircleLines(v.x, v.y, e->view_size * GetScreenHeight()/2, GREEN);
}
