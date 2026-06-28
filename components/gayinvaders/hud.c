#include "hud.h"
#include "gayinvaders.h"
#include "wd.h"

#define HEALTHBAR_START 100
#define AMMO_START      (SCREEN_W-100)

void hud_init(hud_t *hb, int x, int y)
{
	const asset_info_t *ass_inf;
	int i;

	ass_inf = wd_get_asset_info(ASSET_TYPE_HEALTHBARBLOCK);

	for (i = 0; i < HUD_HEALTHBAR_BLOCKS; ++i) {
		hb->blocks[i].ro.parent = &hb->blocks[i].go;
		hb->blocks[i].ro.buff = wd_get_asset(ASSET_TYPE_HEALTHBARBLOCK);
		hb->blocks[i].ro.w = ass_inf->w;
		hb->blocks[i].ro.h = ass_inf->h;

		hb->blocks[i].go.x = HEALTHBAR_START + (ass_inf->w*2) * i;
		hb->blocks[i].go.y = y;

		hb->blocks[i].go.active = true;
	}

	// Bomb
	ass_inf = wd_get_asset_info(ASSET_TYPE_AMMO0);

	hb->bomb.ro.parent = &hb->bomb.go;
	hb->bomb.ro.buff = wd_get_asset(ASSET_TYPE_AMMO0);
	hb->bomb.ro.w = ass_inf->w;
	hb->bomb.ro.h = ass_inf->h;

	hb->bomb.go.x = AMMO_START + (ass_inf->w*2) * 0;
	hb->bomb.go.y = y;

	hb->bomb.go.active = true;

	// Shield
	ass_inf = wd_get_asset_info(ASSET_TYPE_AMMO1);

	hb->shield.ro.parent = &hb->shield.go;
	hb->shield.ro.buff = wd_get_asset(ASSET_TYPE_AMMO1);
	hb->shield.ro.w = ass_inf->w;
	hb->shield.ro.h = ass_inf->h;

	hb->shield.go.x = AMMO_START + (ass_inf->w*2) * 1;
	hb->shield.go.y = y;

	hb->shield.go.active = true;

	// Rapid fire
	ass_inf = wd_get_asset_info(ASSET_TYPE_AMMO2);

	hb->rapidfire.ro.parent = &hb->rapidfire.go;
	hb->rapidfire.ro.buff = wd_get_asset(ASSET_TYPE_AMMO2);
	hb->rapidfire.ro.w = ass_inf->w;
	hb->rapidfire.ro.h = ass_inf->h;

	hb->rapidfire.go.x = AMMO_START + (ass_inf->w*2) * 2;
	hb->rapidfire.go.y = y;

	hb->rapidfire.go.active = true;
}

void hud_destroy(hud_t *hb)
{
	wd_not_using(ASSET_TYPE_HEALTHBARBLOCK);

	if (hb->bomb.go.active) {
		hb->bomb.go.active = false;
		wd_not_using(ASSET_TYPE_AMMO0);
	}

	if (hb->shield.go.active) {
		hb->shield.go.active = false;
		wd_not_using(ASSET_TYPE_AMMO1);
	}

	if (hb->rapidfire.go.active) {
		hb->rapidfire.go.active = false;
		wd_not_using(ASSET_TYPE_AMMO2);
	}
}

void hud_update(hud_t *hb, int health, bool bomb, bool shield,
		bool rapidfire)
{
	int i;

	for (i = 0; i < HUD_HEALTHBAR_BLOCKS; ++i)
		hb->blocks[i].go.active = i+1 <= health;

	hb->bomb.go.active = bomb;
	hb->shield.go.active = shield;
	hb->rapidfire.go.active = rapidfire;
}

void hud_render(hud_t *hb)
{
	int i;

	for (i = 0; i < HUD_HEALTHBAR_BLOCKS; ++i)
		renderer_render(&hb->blocks[i].ro);

	renderer_render(&hb->bomb.ro);
	renderer_render(&hb->shield.ro);
	renderer_render(&hb->rapidfire.ro);
}
