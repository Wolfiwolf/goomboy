#include "hud.h"
#include "gayinvaders.h"
#include "wd.h"

static const uint16_t *_hb_block_image = NULL;
static const uint16_t *_hb_ammo_images[BULLET_PLAYER_SPECIAL_CNT] = {};

#define HEALTHBAR_START 100
#define AMMO_START      (SCREEN_W-100)

void hud_init(hud_t *hb, int x, int y)
{
	const asset_info_t *ass_inf;
	int i;

	ass_inf = wd_get_asset_info(ASSET_TYPE_HEALTHBARBLOCK);

	if (!_hb_block_image)
		_hb_block_image = wd_get_asset(ASSET_TYPE_HEALTHBARBLOCK);
	
	for (i = 0; i < HUD_HEALTHBAR_BLOCKS; ++i) {
		hb->blocks[i].ro.parent = &hb->blocks[i].go;
		hb->blocks[i].ro.buff = _hb_block_image;
		hb->blocks[i].ro.w = ass_inf->w;
		hb->blocks[i].ro.h = ass_inf->h;

		hb->blocks[i].go.x = HEALTHBAR_START + (ass_inf->w*2) * i;
		hb->blocks[i].go.y = y;

		hb->blocks[i].go.active = true;
	}

	for (i = 0; i < BULLET_PLAYER_SPECIAL_CNT; ++i) {
		asset_type_t ass_type = ASSET_TYPE_AMMO0+i;
		const asset_info_t *ass_inf;

		ass_inf = wd_get_asset_info(ass_type);

		if (!_hb_ammo_images[i])
			_hb_ammo_images[i] = wd_get_asset(ass_type);

		hb->ammos[i].ro.parent = &hb->ammos[i].go;
		hb->ammos[i].ro.buff = _hb_ammo_images[i];
		hb->ammos[i].ro.w = ass_inf->w;
		hb->ammos[i].ro.h = ass_inf->h;

		hb->ammos[i].go.x = AMMO_START + (ass_inf->w*2) * i;
		hb->ammos[i].go.y = y;

		hb->ammos[i].go.active = true;
	}
}

void hud_update(hud_t *hb, int health, bool bomb)
{
	int i;

	for (i = 0; i < HUD_HEALTHBAR_BLOCKS; ++i)
		hb->blocks[i].go.active = i+1 <= health;

	hb->ammos[BULLET_TYPE_BOMB-BULLET_PLAYER_SPECIAL_START].go.active = bomb;
}

void hud_render(hud_t *hb)
{
	int i;

	for (i = 0; i < HUD_HEALTHBAR_BLOCKS; ++i)
		renderer_render(&hb->blocks[i].ro);

	for (i = 0; i < BULLET_PLAYER_SPECIAL_CNT; ++i)
	 	renderer_render(&hb->ammos[i].ro);
}
