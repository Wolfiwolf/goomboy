#ifndef WD_H_
#define WD_H_

#include <stdint.h>

typedef enum {
	ASSET_TYPE_NONE = -1,
	ASSET_TYPE_INTRO = 0,
	ASSET_TYPE_LEFTSIDE,
	ASSET_TYPE_RIGHTSIDE,
	ASSET_TYPE_MAINMENUSELECTOR,
	ASSET_TYPE_MAINMENUBTN0,
	ASSET_TYPE_MAINMENUBTN1,
	ASSET_TYPE_MAINMENUBTN2,
	ASSET_TYPE_PLAYER,
	ASSET_TYPE_ENEMYEASYIDLE,
	ASSET_TYPE_ENEMYEASYSHOOT,
	ASSET_TYPE_ENEMYEASYPAIN,
	ASSET_TYPE_ENEMYMOVINGIDLE,
	ASSET_TYPE_ENEMYMOVINGSHOOT,
	ASSET_TYPE_ENEMYMOVINGPAIN,
	ASSET_TYPE_ENEMYFASTIDLE,
	ASSET_TYPE_ENEMYFASTSHOOT,
	ASSET_TYPE_ENEMYFASTPAIN,
	ASSET_TYPE_BULLETNORMAL,
	ASSET_TYPE_BULLETBOMB,
	ASSET_TYPE_ENEMYBULLETNORMAL,
	ASSET_TYPE_BULLETNORMALHIT,
	ASSET_TYPE_BULLETBOMBHIT,
	ASSET_TYPE_ENEMYBULLETNORMALHIT,
	ASSET_TYPE_HEALTHBARBLOCK,
	ASSET_TYPE_AMMO0,
	ASSET_TYPE_POWERUPHEALTH,
	ASSET_TYPE_POWERUPBOMB,
	ASSET_TYPE_COUNT,
} asset_type_t;

typedef struct {
	uint8_t name_size;
	char name[32];
	uint32_t size;
	uint32_t offset;
	uint32_t w;
	uint32_t h;

	int consumers;
} asset_info_t;

int wd_init(const char *wd_fpath);

const asset_info_t *wd_get_asset_info(asset_type_t atype);

int wd_read_asset_direct(asset_type_t atype, uint16_t *buffer,
			 int xoff, int yoff,
			 int w, int h);

void wd_not_using(asset_type_t atype);

const uint16_t *wd_get_asset(asset_type_t atype);

#endif
