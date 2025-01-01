#ifndef _ASSETS_H_
#define _ASSETS_H_

#include <raylib.h>

typedef struct
{
	int assets_count;

	struct asset_data_t
	{
		enum { ASSET_IMAGE } type;
		Image image;
		Color *colors;
	} *data;
} assets_t;

int assets_load_image(assets_t *as, const char *c);
void assets_unload(assets_t *as);

#endif
