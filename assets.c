#include "assets.h"
#include "util.h"
#include <assert.h>

int assets_load_image(assets_t *as, const char *fil)
{
	Texture2D img = LoadTexture(fil);

	if(!IsTextureValid(img))
	{
		return -1;
	}

	struct asset_data_t dat = {
		.type = ASSET_IMAGE,
		.image = img,
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
				UnloadTexture(as->data[i].image);
				break;
		}
	}

	free(as->data);
}
