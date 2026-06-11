#ifndef WD_H_
#define WD_H_

#include <stdint.h>

typedef enum {
	ASSET_TYPE_NONE = -1,
	ASSET_TYPE_INTRO = 0,
	ASSET_TYPE_LEFTSIDE,
	ASSET_TYPE_RIGHTSIDE,
	ASSET_TYPE_PLAYER,
	ASSET_TYPE_BULLET,
	ASSET_TYPE_COUNT,
} asset_type_t;

typedef struct {
	uint8_t name_size;
	char name[32];
	uint32_t size;
	uint32_t offset;
	uint32_t w;
	uint32_t h;
} asset_info_t;

int wd_init(const char *wd_fpath);

const asset_info_t *wd_get_asset_info(asset_type_t atype);

int wd_read_asset(asset_type_t atype, uint16_t *buffer, int xoff, int yoff, int w, int h);

#endif
