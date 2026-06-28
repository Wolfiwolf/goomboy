#ifndef HUD_H_
#define HUD_H_

#include "gameobject.h"
#include "renderer.h"

#define HUD_HEALTHBAR_BLOCKS 10

typedef struct {
	game_object_t go;
	render_obj_t ro;
} hud_health_block_t;

typedef struct {
	game_object_t go;
	render_obj_t ro;
} hud_ammo_t;

typedef struct {
	hud_health_block_t blocks[HUD_HEALTHBAR_BLOCKS];
	hud_ammo_t bomb;
	hud_ammo_t shield;
	hud_ammo_t rapidfire;
} hud_t;

void hud_init(hud_t *hb, int x, int y);

void hud_destroy(hud_t *hb);

void hud_update(hud_t *hb, int health, bool bomb, bool shield,
		bool rapidfire);

void hud_render(hud_t *hb);

#endif
