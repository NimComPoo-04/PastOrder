#include "map.h"
#include "draw_ext.h"
#include "util.h"

#include <rlgl.h>
#include <stdio.h>

void map_load_data(map_t *m, const char *file)
{
	FILE *f = fopen(file, "rb+");

	int current_sector = -1;

	static char line[1024] = {0};

	while(!feof(f))
	{
		fgets(line, sizeof line, f);

		switch(line[0])
		{
			case 'v':
				{
					vertex_t v = {0};

					sscanf(line+1, "%f %f", &v.pos.x, &v.pos.y);

					m->vertices = array_append(m->vertices,
							&m->vertex_count, &v, sizeof v);

					break;
				}

			case 's':
				{
					sector_t sect = {0};
					m->sectors = array_append(m->sectors, &m->sectors_count,
							&sect, sizeof(sect));
					current_sector++;
					break;
				}

			case 'w':
				{
					line_t t = {0};
					sscanf(line+1, "%i %i %hhx %hhx %hhx %hhx %hhx %hhx",
							&t.start, &t.end,
							&t.col_start.r, &t.col_start.g, &t.col_start.b,
							&t.col_end.r, &t.col_end.g, &t.col_end.b);

					t.col_start.a = 255;
					t.col_end.a = 255;

					sector_t *s = &m->sectors[current_sector];
					s->walls = array_append(s->walls, &s->walls_count, &t, sizeof(t));

					break;
				}

			case 'p':
				{
					line_t t = {0};
					int neighbour;
					sscanf(line+1, "%i %i %i", &t.start, &t.end, &neighbour);

					sector_t *s = &m->sectors[current_sector];

					int botch = s->portals_count;
					s->portals = array_append(s->portals, &s->portals_count, &t, sizeof(t));
					s->neighbours = array_append(s->neighbours, &botch, &neighbour, sizeof(neighbour));

					break;
				}

			case 'e':
				{
					float elevation;
					sscanf(line+1, "%f", &elevation);

					sector_t *s = &m->sectors[current_sector];
					s->elevation= elevation;

					break;
				}

			case 'h':
				{
					float height;
					sscanf(line+1, "%f", &height);

					sector_t *s = &m->sectors[current_sector];
					s->height = height;

					break;
				}

			case 'P':
				{
					Vector2 point;

					sscanf(line+1, "%f %f", &point.x, &point.y);

					m->player->sectId = current_sector;
					m->player->pos = point;

					break;
				}

			case 'E':
				{
					entity_t e = {0};
					sscanf(line+1, "%f %f", &e.pos.x, &e.pos.y);
					e.size = 0.1;
					e.angle = 0;
					e.vel = 1.5;
					e.fov = PI/2;
					e.eye = 0.1;
					e.sectId = current_sector;

					m->entities = array_append(m->entities, &m->entities_count, &e, sizeof(e));

					break;
				}

		}
	}

	fclose(f);
}

void map_dump(map_t *m)
{
	printf("p\t%f %f\t%i\n", m->player->pos.x, m->player->pos.y, m->player->sectId);
	puts("");

	for(int i = 0; i < m->vertex_count; i++)
		printf("v\t%f %f\n",
				m->vertices[i].pos.x, m->vertices[i].pos.y);
	puts("");

	for(int i = 0; i < m->sectors_count; i++)
	{
		printf("s\n");

		printf("e\t%f\n", m->sectors[i].elevation);
		printf("h\t%f\n", m->sectors[i].height);

		for(int j = 0; j < m->sectors[i].walls_count; j++)
		{
			printf("w\t%i %i\t%2x %2x %2x\t%2x %2x %2x\n",
					m->sectors[i].walls[j].start, m->sectors[i].walls[j].end,
					m->sectors[i].walls[j].col_start.r, m->sectors[i].walls[j].col_start.g, m->sectors[i].walls[j].col_start.b,
					m->sectors[i].walls[j].col_end.r, m->sectors[i].walls[j].col_end.g, m->sectors[i].walls[j].col_end.b);
		}

		for(int j = 0; j < m->sectors[i].portals_count; j++)
			printf("p\t%i %i\t%i\n",
					m->sectors[i].portals[j].start, m->sectors[i].portals[j].end,
					m->sectors[i].neighbours[j]);
		puts("");
	}
}

Vector2 map_to_screen(map_t *m, vertex_t v)
{
	int wid = GetScreenWidth();
	int hei = GetScreenHeight();

	Vector2 world_screen = {wid/(2. * wid/hei), hei/2.};
	Vector2 world_offset = {wid/2., hei/2.};

	Vector2 view_v = Vector2Rotate(Vector2Subtract(v.pos, m->player->pos), -m->player->angle);
	Vector2 wrld_v = Vector2Add(world_offset, Vector2Multiply(world_screen, view_v));

	return wrld_v;
}

static inline Vector2 map_to_world(map_t *m, Vector2 v)
{
	(void)m;
	int wid = GetScreenWidth();
	int hei = GetScreenHeight();

	Vector2 world_screen = {wid/(2. * wid/hei), hei/2.};
	Vector2 world_offset = {wid/2., hei/2.};

	return Vector2Add(world_offset, Vector2Multiply(world_screen, v));
}

static inline Vector2 map_to_relpalyer(map_t *m, vertex_t v)
{
	return Vector2Rotate(Vector2Subtract(v.pos, m->player->pos), -m->player->angle);
}

void map_draw_vertices(map_t *m)
{
	for(int i = 0; i < m->vertex_count; i++)
	{
		Vector2 v = map_to_screen(m, m->vertices[i]);

		DrawRectangleLines(v.x - 3, v.y - 3, 6, 6, GREEN);
		DrawCircle(v.x, v.y, 4, RED);
	}
}

void map_draw_walls(map_t *m)
{
	for(int j = 0; j < m->sectors_count; j++)
	{
		sector_t s = m->sectors[j];
		for(int i = 0; i < s.walls_count; i++)
		{
			line_t l = s.walls[i];

			Vector2 a = map_to_screen(m, m->vertices[l.start]);
			Vector2 b = map_to_screen(m, m->vertices[l.end]);

			DrawLineV(a, b, MAROON);
		}
	}
}

static inline float ray_line_intersection(Vector2 o, Vector2 n, Vector2 a, Vector2 b)
{
	Vector2 ab = Vector2Subtract(b, a);
	Vector2 oa = Vector2Subtract(a, o);

	float C = oa.y/n.y - oa.x/n.x;
	float M = ab.y/n.y - ab.x/n.x;

	float t = -C/M;

	return t;
}

static inline int clip_points(map_t *m, line_t l, Vector2 fov1, Vector2 fov2, Vector2 pts[2], Color col[2], float tex[2])
{
	int pts_len = 0;

	Vector2 a = map_to_relpalyer(m, m->vertices[l.start]);
	Vector2 b = map_to_relpalyer(m, m->vertices[l.end]);

	// Clipping code

	float ang_a = atan2(a.y, a.x);
	float ang_b = atan2(b.y, b.x);

	float ang_fov1 = atan2(fov1.y, fov1.x);
	float ang_fov2 = atan2(fov2.y, fov2.x);

	if(IsKeyPressed(KEY_U))
	TraceLog(LOG_INFO, "%f %f %f %f", ang_a, ang_b, ang_fov1, ang_fov2);

	if(ang_fov1 > ang_fov2)
	{
		swap(ang_fov1, ang_fov2, float);
		swap(fov1, fov2, Vector2);
	}

	if(ang_a >= ang_fov1 && ang_a <= ang_fov2)
	{
		col[pts_len] = l.col_start;
		tex[pts_len] = 0;
		pts[pts_len++] = a;
	}

	if(ang_b >= ang_fov1 && ang_b <= ang_fov2)
	{
		col[pts_len] = l.col_end;
		tex[pts_len] = 1;
		pts[pts_len++] = b;
	}

	if(pts_len < 2)
	{
		float t1 = ray_line_intersection(Vector2Zero(), fov1, a, b);
		float t2 = ray_line_intersection(Vector2Zero(), fov2, a, b);

		if(t1 >= 0 && t1 <= 1)
		{
			Vector2 v = Vector2Lerp(a, b, t1);
			if(v.x > 0)
			{
				col[pts_len] = ColorLerp(l.col_start, l.col_end, t1);
				tex[pts_len] = t1;
				pts[pts_len++] = v;
			}
		}

		if(t2 >= 0 && t2 <= 1)
		{
			Vector2 v = Vector2Lerp(a, b, t2);
			if(v.x > 0)
			{
				col[pts_len] = ColorLerp(l.col_start, l.col_end, t2);
				tex[pts_len] = t2;
				pts[pts_len++] = v;
			}
		}
	}

	return pts_len;
}

void draw_3d_view(map_t *m, Vector2 *pts, Color *col, float *tex, int wid, int hei, sector_t s, sector_t ns,
		int isportal, float *y_top, float *y_bottom)
{
	//float arc = tan(m->player->fov/2);
	float znear = 1;

	float z1 = pts[0].x;
	float z2 = pts[1].x;

	float x1 = clamp((pts[0].y * znear / z1 + 1)/2, 0.0015, 1) * wid;
	float x2 = clamp((pts[1].y * znear / z2 + 1)/2, 0.0015, 1) * wid;

	if(x1 > x2)
	{
		swap(x1, x2, float);
		swap(z1, z2, float);
		swap(tex[0], tex[1], float);
		swap(col[0], col[1], Color);
	}

	float ty1 = hei - ((s.height - s.elevation - m->player->eye) * znear / z1 + 1)/2 * hei;
	float ty2 = hei - ((s.height - s.elevation - m->player->eye) * znear / z2 + 1)/2 * hei;
	float by1 = hei - ((s.elevation - m->player->eye) * znear / z1 + 1)/2 * hei;
	float by2 = hei - ((s.elevation - m->player->eye) * znear / z2 + 1)/2 * hei;

	float nty1 = hei - ((ns.height - ns.elevation - m->player->eye) * znear / z1 + 1)/2 * hei;
	float nty2 = hei - ((ns.height - ns.elevation - m->player->eye) * znear / z2 + 1)/2 * hei;
	float nby1 = hei - ((ns.elevation - m->player->eye) * znear / z1 + 1)/2 * hei;
	float nby2 = hei - ((ns.elevation - m->player->eye) * znear / z2 + 1)/2 * hei;

	if((int)x1 == (int)x2) return;

	float dty = (ty2 - ty1)/(x2 - x1);
	float dby = (by2 - by1)/(x2 - x1);

	float ndty = (nty2 - nty1)/(x2 - x1);
	float ndby = (nby2 - nby1)/(x2 - x1);

	float w1 = 1./z1;
	float w2 = 1./z2;

	float t = 0;
	float dt = 1/(x2 - x1);

	float dz = 1/(z2 - z1);

	for(float x = x1, ty = ty1, by = by1, nty = nty1, nby = nby1, z = z1;
			x <= x2;
			x++, ty += dty, by += dby, z += dz)
	{
		float tmp_ty = clamp(ty, y_top[(int)x], y_bottom[(int)x]);
		float tmp_by = clamp(by, y_top[(int)x], y_bottom[(int)x]);

		DrawLine(x, tmp_ty, x, y_top[(int)x], LIGHTGRAY);
		DrawLine(x, tmp_by, x, y_bottom[(int)x], DARKPURPLE);

		if(isportal)
		{
			float tmp_nty = clamp(nty, y_top[(int)x], y_bottom[(int)x]);
			float tmp_nby = clamp(nby, y_top[(int)x], y_bottom[(int)x]);

			tmp_nty = nty > ty ? nty : ty;
			tmp_nby = nby < by ? nby : by;

			DrawLine(x, tmp_ty, x, tmp_nty , PINK);
			DrawLine(x, tmp_by, x, tmp_nby , PURPLE);

			y_top[(int)x] = tmp_nty < y_top[(int)x] ? y_top[(int)x] : tmp_nty;
			y_bottom[(int)x] = tmp_nby  > y_bottom[(int)x] ? y_bottom[(int)x] : tmp_nby ;

			nty += ndty;
			nby += ndby;
		}
		else
		{
			float up = (tmp_ty - ty)/(by - ty);
			float down = (tmp_by - ty)/(by - ty);

			rlSetTexture(m->assets->data[0].image.id);
			rlBegin(RL_QUADS);

				Color c = ColorLerp(col[0], col[1], t);

				rlColor4ub(c.r, c.g, c.b, 255);
				rlNormal3f(0, 0, 1);

				float w = (1 - t) * w1 + t * w2;
				float u = (1 - t) * tex[0] * w1 + t * tex[1] * w2;
				u = u / w;

				rlTexCoord2f(u, up); rlVertex2f(x  , tmp_ty);
				rlTexCoord2f(u, down); rlVertex2f(x  , tmp_by);
				rlTexCoord2f(u, down); rlVertex2f(x+1, tmp_by);
				rlTexCoord2f(u, up); rlVertex2f(x+1, tmp_ty);

			rlEnd();
			rlSetTexture(0);

			y_top[(int)x] = ty < y_top[(int)x] ? y_top[(int)x] : ty;
			y_bottom[(int)x] = by > y_bottom[(int)x] ? y_bottom[(int)x] : by;
		}

		t += dt;
	}
}

void map_draw_sectors(map_t *m)
{
	int wid = GetScreenWidth();
	int hei = GetScreenHeight();

	char *visited_sectors = calloc(m->sectors_count, sizeof(char));

	// Setting up the y thing
	float *y_top = malloc(sizeof(float) * GetScreenWidth());
	float *y_bottom = malloc(sizeof(float) * GetScreenWidth());

	for(int i = 0; i < GetScreenWidth(); i++)
	{
		y_top[i] = 0;
		y_bottom[i] = hei;
	}

	enum { MaxQueue = 1024 };

	static struct
	{
		int sectId;
		Vector2 fov1;
		Vector2 fov2;
	} queue[MaxQueue] = {0};

	int head = 0;
	int tail = 0;

	queue[tail].sectId = m->player->sectId;
	queue[tail].fov1 = (Vector2){cos(m->player->fov/2), sin(m->player->fov/2)};
	queue[tail].fov2 = (Vector2){queue[tail].fov1.x, -queue[tail].fov1.y};

	tail++;

	Vector2 pts[4];
	Color col[4];
	float tex[4];

	if(m->debug_view)
	{
		map_draw_vertices(m);
		map_draw_walls(m);

		player_draw(m->player);

		for(int i = 0; i < m->entities_count; i++)
			entity_draw(&m->entities[i], m);
	}

	while(head != tail)
	{
		int sectId = queue[head].sectId;
		sector_t s = m->sectors[sectId];
		Vector2 fov1 = queue[head].fov1;
		Vector2 fov2 = queue[head].fov2;

		head = (head + 1) % MaxQueue;

		for(int i = 0; i < s.walls_count; i++)
		{
			line_t l = s.walls[i];

			int pts_len = clip_points(m, l, fov1, fov2, pts, col, tex);

			if(pts_len != 2)
				continue;

			if(m->debug_view)
			{
				Vector2 ka = map_to_world(m, pts[0]);
				Vector2 kb = map_to_world(m, pts[1]);

				DrawLineGradientV(ka, col[0], kb, col[1]);

				Vector2 v = {GetScreenWidth()/2, GetScreenHeight()/2};

				DrawLineV(v, ka, GREEN);
				DrawLineV(v, kb, GREEN);
			}
			else
				draw_3d_view(m, pts, col, tex, wid, hei, s, s, 0, y_top, y_bottom);
		}

		visited_sectors[sectId] = 1;

		for(int i = 0; i < s.portals_count; i++)
		{
			line_t l = s.portals[i];

			int pts_len = clip_points(m, l, fov1, fov2, pts, col, tex);

			if(pts_len != 2) continue;
			if(visited_sectors[s.neighbours[i]] != 0) continue;

			sector_t ns = m->sectors[s.neighbours[i]];

			if(m->debug_view)
			{
				Vector2 ka = map_to_world(m, pts[0]);
				Vector2 kb = map_to_world(m, pts[1]);

				DrawLineGradientV(ka, col[0], kb, col[1]);
			}
			else
				draw_3d_view(m, pts, col, tex, wid, hei, s, ns, 1, y_top, y_bottom);

			queue[tail].sectId = s.neighbours[i];
			queue[tail].fov1 = pts[0];
			queue[tail].fov2 = pts[1];

			queue[tail].fov1.y -= 0.00001 * sign(queue[tail].fov1.y);
			queue[tail].fov2.y -= 0.00001 * sign(queue[tail].fov2.y);

			tail = (tail + 1) % MaxQueue;

			visited_sectors[s.neighbours[i]] = 1;
		}
	}

	free(y_top);
	free(y_bottom);
	free(visited_sectors);
}

void map_update(map_t *m)
{
	if(IsKeyPressed(KEY_D))
		m->debug_view = !m->debug_view;

	for(int i = 0; i < m->entities_count; i++)
		entity_update(&m->entities[i], m);
}
