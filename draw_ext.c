#include "draw_ext.h"
#include <rlgl.h>

void DrawLineGradientV(Vector2 a, Color ca, Vector2 b, Color cb)
{
	rlBegin(RL_LINES);
		rlColor4ub(ca.r, ca.g, ca.b, ca.a);
		rlVertex2f(a.x, a.y);
		rlColor4ub(cb.r, cb.g, cb.b, cb.a);
		rlVertex2f(b.x, b.y);
	rlEnd();
}
