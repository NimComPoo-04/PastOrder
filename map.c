#include "map.h"

static float intersect_wall_ray(Vector2 start, Vector2 end, float angle)
{
	float slope = tan(angle);

	if(isinf(slope) || isnan(slope)) return -1;

	float m = (end.y - start.y)/(end.x - start.x);

	float x = ( end.y - m * end.x ) / ( slope - m );
	float y = slope * x;

	if(y > 0)
		return -1;

	return (x - start.x) / (end.x - start.x);
}

void walls_debug_draw(wall_t *walls, int count, player_t *p)
{
	Vector2 start = {0, 0}, end = {0, 0};

	int width = GetScreenWidth();
	int height = GetScreenHeight();

	Vector2 world_transform = { width / 2.f, height / 2.f };

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
			float t = intersect_wall_ray(start, end, i * p->fov/2);

			if(t > 0 && t < 1)
			{
				if(IsKeyDown(KEY_U))
					TraceLog(LOG_WARNING, "Value of T = %lf", t);

				Vector2 point = Vector2Lerp(start, end, t);

				int flag = 1;
				if(Vector2LengthSqr(point) < p->size * p->size)
					flag = 0;

				point = Vector2Multiply(point, world_transform);
				point = Vector2Add(point, world_transform);

				if(flag)
					drawpoints[drawpoints_len++] = point;

				DrawCircleV(point, 3, ORANGE);
			}
		}

		// Drawing shit
		start = Vector2Multiply(start, world_transform);
		start = Vector2Add(start, world_transform);

		end = Vector2Multiply(end, world_transform);
		end = Vector2Add(end, world_transform);

		DrawLineEx(start, end, 3, walls[i].isportal ? SKYBLUE : MAROON);

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

	Vector2 window_scale_offset = { width/2.f, height/2.f };

	Vector2 dirvec = {0, -40};

	DrawLineEx(window_scale_offset, Vector2Add(window_scale_offset, dirvec), 3, BLUE);
	DrawCircleV(window_scale_offset, 5, RED);
	DrawCircleLinesV(window_scale_offset, player->size * height/2, MAROON);

	DrawLineEx(window_scale_offset,
			Vector2Add(window_scale_offset, Vector2Rotate(dirvec, -player->fov/2)),
			3, YELLOW);

	DrawLineEx(window_scale_offset,
			Vector2Add(window_scale_offset, Vector2Rotate(dirvec, player->fov/2)),
			3, YELLOW);
}

int player_wall_collision(player_t *p, wall_t *w)
{
	// Equation of circle (x - x0)^2 + (y - y0)^2 = r^2
	// Equation of Ray    startx + m * t	

	if(w->isportal)
		return 0;

	Vector2 a = w->point_start;
	a = Vector2Subtract(a, p->pos);
	a = Vector2Rotate(a, -p->angle);

	Vector2 q = w->point_end;	
	q = Vector2Subtract(q, p->pos);
	q = Vector2Rotate(q, -p->angle);

	float r = p->size;
	Vector2 d = Vector2Normalize(Vector2Subtract(q, a));

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
