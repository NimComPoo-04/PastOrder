#include "assets.h"
#include "util.h"

int assets_load_image(assets_t *as, const char *fil)
{
	Image img = LoadImage(fil);

	struct asset_data_t dat = {
		.type = ASSET_IMAGE,
		.image = img,
		.colors = LoadImageColors(img)
	};

	as->data = array_append(as->data, &as->assets_count, &dat, sizeof(struct asset_data_t));
	return as->assets_count - 1;
}

void assets_unload(assets_t *as)
{
	for(int i = 0; i < as->assets_count; i++)
	{
		switch(as->data[i].type)
		{
			case ASSET_IMAGE:
				UnloadImage(as->data[i].image);
				UnloadImageColors(as->data[i].colors);
				break;
		}
	}

	free(as->data);
}
