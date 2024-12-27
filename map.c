#include <memory.h>
#include "map.h"

static float intersect_wall_ray(Vector2 start, Vector2 end, float angle, Vector2 *out)
{
	float slope = tan(angle);

	if(isinf(slope) || isnan(slope)) return -1;

	float m = (end.y - start.y)/(end.x - start.x);

	float x = ( end.y - m * end.x ) / ( slope - m );
	float y = slope * x;

	if(out)
	{
		out->x = x;
		out->y = y;
	}

	if(y > 0)
		return -1;

	return (x - start.x) / (end.x - start.x);
}

void walls_debug_draw(wall_t *walls, int count, player_t *p)
{
	Vector2 start = {0, 0}, end = {0, 0};

	int width = GetScreenWidth();
	int height = GetScreenHeight();

	Vector2 world_scale = { width / (2.f * width/height), height / 2.f };
	Vector2 world_offset = { width / 2.f, height / 2.f };

	Vector2 fovvec1 = {cos(-p->fov/2 + p->angle), sin(-p->fov/2 + p->angle)};
	Vector2 fovvec2 = {cos(p->fov/2 + p->angle), sin(p->fov/2 + p->angle)};

	for(int i = 0; i < count; i++)
	{
		Vector2 drawpoints[2] = {0};
		int drawpoints_len = 0;

		start = walls[i].point_start;
		start = Vector2Subtract(start, p->pos);
		start = Vector2Rotate(start, -p->angle);

		end = walls[i].point_end;
		end = Vector2Subtract(end, p->pos);
		end = Vector2Rotate(end, -p->angle);

		for(int i = -1; i <= 1; i += 2)
		{
			float t = intersect_wall_ray(start, end, i * p->fov/2, NULL);

			if(t > 0 && t < 1)
			{
				Vector2 point = Vector2Lerp(start, end, t);

				int flag = 1;
				if(Vector2LengthSqr(point) < p->size * p->size)
					flag = 0;

				point = Vector2Multiply(point, world_scale);
				point = Vector2Add(point, world_offset);

				if(flag)
					drawpoints[drawpoints_len++] = point;

				DrawCircleV(point, 3, ORANGE);
			}
		}

		// Drawing shit
		start = Vector2Multiply(start, world_scale);
		start = Vector2Add(start, world_offset);

		end = Vector2Multiply(end, world_scale);
		end = Vector2Add(end, world_offset);

		DrawLineEx(start, end, 3, walls[i].isportal ? GRAY : MAROON);

		Vector2 sub = Vector2Subtract(walls[i].point_start, p->pos);
		if(Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0)
		{
			drawpoints[drawpoints_len++] = start;
			DrawCircleV(start, 3, ORANGE);
		}

		sub = Vector2Subtract(walls[i].point_end, p->pos);
		if(Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0)
		{
			drawpoints[drawpoints_len++] = end;
			DrawCircleV(end, 3, ORANGE);
		}

		if(drawpoints_len == 2)
		{
			DrawLineV(drawpoints[0], drawpoints[1], SKYBLUE);
		}
	}
}

void player_debug_draw(player_t *player)
{
	int width = GetScreenWidth();
	int height = GetScreenHeight();

//	Vector2 window_scale = { width/(2.f * width / height), height/2.f };
	Vector2 window_offset = { width/2.f, height/2.f };

	Vector2 dirvec = {0, -40};

	DrawLineEx(window_offset, Vector2Add(window_offset, dirvec), 3, BLUE);
	DrawCircleV(window_offset, 5, RED);
	DrawCircleLinesV(window_offset, player->size * height/2, MAROON);

	DrawLineEx(window_offset,
			Vector2Add(window_offset, Vector2Rotate(dirvec, -player->fov/2)),
			3, YELLOW);

	DrawLineEx(window_offset,
			Vector2Add(window_offset, Vector2Rotate(dirvec, player->fov/2)),
			3, YELLOW);
}

int player_wall_collision(player_t *p, wall_t *w)
{
	// Equation of circle (x - x0)^2 + (y - y0)^2 = r^2
	// Equation of Ray    startx + m * t	

	Vector2 a = w->point_start;
	a = Vector2Subtract(a, p->pos);
	a = Vector2Rotate(a, -p->angle);

	Vector2 q = w->point_end;	
	q = Vector2Subtract(q, p->pos);
	q = Vector2Rotate(q, -p->angle);

	float r = p->size;
	Vector2 d = Vector2Subtract(q, a);

	float c_ = Vector2LengthSqr(a) - r * r;
	float b_ = 2 * Vector2DotProduct(a, d);
	float a_ = Vector2LengthSqr(d);

	float disc = b_ * b_ - 4 * a_ * c_;

	if(disc < 0)
		return 0;

	float t0 = (-b_ + sqrt(disc)) / (2 * a_);
	float t1 = (-b_ - sqrt(disc)) / (2 * a_);

	if(0 <= t0 && t0 <= 1)
		return 1;

	if(0 <= t1 && t1 <= 1)
		return 1;

	return 0;
}

// Draw only the sectors which are visible
void sector_debug_draw(sector_t *s, int count, player_t *p, wall_t *walls, int walls_count)
{
	(void)count;

	Vector2 fovvec1 = {cos(-p->fov/2 + p->angle), sin(-p->fov/2 + p->angle)};
	Vector2 fovvec2 = {cos(p->fov/2 + p->angle), sin(p->fov/2 + p->angle)};

	int visited[walls_count];
	memset(visited, 0, sizeof(visited));

	int queue[32] = {0};
	int head = 0;
	int tail = 0;

	queue[tail] = p->sectId;
	tail = (tail + 1) % 32;

	while(head != tail)
	{
		int i = queue[head];
		head = (head + 1) % 32;

		for(int j = 0; j < s[i].walls_count; j++)
		{
			walls_debug_draw(&walls[s[i].walls[j]], 1, p);

			if(walls[s[i].walls[j]].isportal)
			{
				int isinrange = 0;

				Vector2 start = walls[s[i].walls[j]].point_start;
				start = Vector2Subtract(start, p->pos);
				start = Vector2Rotate(start, -p->angle);

				Vector2 end = walls[s[i].walls[j]].point_end;
				end = Vector2Subtract(end, p->pos);
				end = Vector2Rotate(end, -p->angle);

				float t = intersect_wall_ray(start, end, p->fov/2, NULL);
				isinrange = isinrange || (t >= 0 && t <= 1);

				t = intersect_wall_ray(start, end, -p->fov/2, NULL);
				isinrange = isinrange || (t >= 0 && t <= 1);

				Vector2 sub = Vector2Subtract(walls[s[i].walls[j]].point_start, p->pos);
				isinrange = isinrange || (Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0);

				sub = Vector2Subtract(walls[s[i].walls[j]].point_end, p->pos);
				isinrange = isinrange || (Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0);

				if(isinrange && !visited[j])
				{
					queue[tail] = s[i].neighbour_sectors[j];
					tail = (tail + 1) % 32;
					visited[j] = 1;
				}
			}
		}
	}
}

void player_update(player_t *player, sector_t *sect, wall_t *walls)
{
	float dt = GetFrameTime();
	float da = -PI/2;

	Vector2 update = {0, 0};

	if(IsKeyDown(KEY_LEFT))
	{
		player->angle -= da * dt;
		update = Vector2Zero();
	}
	if(IsKeyDown(KEY_RIGHT))
	{
		player->angle += da * dt;
		update = Vector2Zero();
	}
	if(IsKeyDown(KEY_UP))
	{
		update =  (Vector2){
			cos(player->angle - PI/2) * player->vel * dt,
				sin(player->angle - PI/2) * player->vel * dt
		};
	}
	if(IsKeyDown(KEY_DOWN))
	{
		update =  (Vector2){
			-cos(player->angle - PI/2) * player->vel * dt,
				-sin(player->angle - PI/2) * player->vel * dt
		};

	}

	player->pos = Vector2Add(player->pos, update);
	for(int j = 0; j < sect[player->sectId].walls_count; j++)
	{
		int i = sect[player->sectId].walls[j];

		if(walls[i].isportal)
		{
			Vector2 dir = Vector2Subtract(walls[i].point_end, walls[i].point_start);
			Vector2 plr = Vector2Subtract(player->pos, walls[i].point_start);

			float k1 = dir.x * plr.y - dir.y * plr.x;

			player->pos = Vector2Subtract(player->pos, update);
			plr = Vector2Subtract(player->pos, walls[i].point_start);

			float k2 = dir.x * plr.y - dir.y * plr.x;

			player->pos = Vector2Add(player->pos, update);

			if(k1 * k2 < 0)
			{
				player->sectId = sect[player->sectId].neighbour_sectors[j];
				break;
			}
		}
		else if(player_wall_collision(player, walls+i))
		{
			player->pos = Vector2Subtract(player->pos, update);

			// FIXME: Buggy wall slide code
			/*
			Vector2 wallvec = Vector2Subtract(walls[i].point_end, walls[i].point_start);
			Vector2 nwallvc = Vector2Normalize(wallvec);

			float t = nwallvc.x * update.y - nwallvc.y * update.x;
			t = (t == 0 ? 0 : (t < 0 ? -1 : 1));

			Vector2 movevec = Vector2Scale(nwallvc, t * Vector2DotProduct(wallvec, update));

			player->pos = Vector2Add(player->pos, movevec);
			update = movevec; j = 0;
			*/
		}
	}
}

// FIXME: pull a full 3d here man 
void wall_draw_3d(wall_t *wall, player_t *p, float elv, float heh)
{
	Vector2 start = {0, 0}, end = {0, 0};

	int width = GetScreenWidth();
	int height = GetScreenHeight();

	Vector2 world_scale = { width / (2.f * (width / height)), height / 2.f };
	Vector2 world_offset = { width / 2.f, height / 2.f };

	Vector2 fovvec1 = {cos(-p->fov/2 + p->angle), sin(-p->fov/2 + p->angle)};
	Vector2 fovvec2 = {cos(p->fov/2 + p->angle), sin(p->fov/2 + p->angle)};

	Vector2 drawpoints[4] = {0};
	int drawpoints_len = 0;

	start = wall->point_start;
	start = Vector2Subtract(start, p->pos);
	start = Vector2Rotate(start, -p->angle);

	end = wall->point_end;
	end = Vector2Subtract(end, p->pos);
	end = Vector2Rotate(end, -p->angle);

	for(int i = -1; i <= 1; i += 2)
	{
		Vector2 point;
		float t = intersect_wall_ray(start, end, i * p->fov/2, &point);

		if(t > 0 && t < 1)
		{
			drawpoints[drawpoints_len++] = point;
		}
	}

	Vector2 sub = Vector2Subtract(wall->point_start, p->pos);
	if(Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0)
	{
		drawpoints[drawpoints_len++] = start;
	}

	sub = Vector2Subtract(wall->point_end, p->pos);
	if(Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0)
	{
		drawpoints[drawpoints_len++] = end;
	}

	if(drawpoints_len != 2) return;

	// Constants

	float z1 = drawpoints[0].y;
	float z2 = drawpoints[1].y;

	float as = tanf(p->fov/2);

	float t1 = (heh - elv - p->eyelvl) / z1 * world_scale.y + world_offset.y;
	float b1 = (elv - p->eyelvl) / z1 * world_scale.y + world_offset.y;

	float t2 = (heh - elv - p->eyelvl) / z2 * world_scale.y + world_offset.y;
	float b2 = (elv - p->eyelvl) / z2 * world_scale.y + world_offset.y;

	float x1 = drawpoints[0].x / z1 * world_scale.x + world_offset.x;
	float x2 = drawpoints[1].x / z2 * world_scale.x + world_offset.x;

	DrawLine(x1, t1, x1, b1, RED);
	DrawLine(x2, t2, x2, b2, RED);

	DrawLine(x1, t1, x2, t2, RED);
	DrawLine(x1, b1, x2, b2, RED);
}

void sector_draw_3d(sector_t *s, int count, player_t *p, wall_t *walls, int walls_count)
{
	(void)count;

	Vector2 fovvec1 = {cos(-p->fov/2 + p->angle), sin(-p->fov/2 + p->angle)};
	Vector2 fovvec2 = {cos(p->fov/2 + p->angle), sin(p->fov/2 + p->angle)};

	int visited[walls_count];
	memset(visited, 0, sizeof(visited));

	int queue[32] = {0};
	int head = 0;
	int tail = 0;

	queue[tail] = p->sectId;
	tail = (tail + 1) % 32;

	while(head != tail)
	{
		int i = queue[head];
		head = (head + 1) % 32;

		for(int j = 0; j < s[i].walls_count; j++)
		{
			wall_draw_3d(&walls[s[i].walls[j]], p, s[i].elevation, s[i].height);

			if(walls[s[i].walls[j]].isportal)
			{
				int isinrange = 0;

				Vector2 start = walls[s[i].walls[j]].point_start;
				start = Vector2Subtract(start, p->pos);
				start = Vector2Rotate(start, -p->angle);

				Vector2 end = walls[s[i].walls[j]].point_end;
				end = Vector2Subtract(end, p->pos);
				end = Vector2Rotate(end, -p->angle);

				float t = intersect_wall_ray(start, end, p->fov/2, NULL);
				isinrange = isinrange || (t >= 0 && t <= 1);

				t = intersect_wall_ray(start, end, -p->fov/2, NULL);
				isinrange = isinrange || (t >= 0 && t <= 1);

				Vector2 sub = Vector2Subtract(walls[s[i].walls[j]].point_start, p->pos);
				isinrange = isinrange || (Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0);

				sub = Vector2Subtract(walls[s[i].walls[j]].point_end, p->pos);
				isinrange = isinrange || (Vector2DotProduct(sub, fovvec1) >= 0 && Vector2DotProduct(sub, fovvec2) <= 0);

				/*
				if(isinrange && !visited[j])
				{
					queue[tail] = s[i].neighbour_sectors[j];
					tail = (tail + 1) % 32;
					visited[j] = 1;
				}
				*/
			}
		}
	}
}
