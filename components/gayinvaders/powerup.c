#include "powerup.h"
#include "gayinvaders.h"
#include "physics.h"
#include "wd.h"

typedef struct {
	int speed;
	asset_type_t image;
} powerup_type_conf_t;

static powerup_type_conf_t _configs[POWERUP_TYPE_CNT] = {
	{ // HEALTH
		.speed = 50,
		.image = ASSET_TYPE_POWERUPHEALTH
	},
	{ // BOMB
		.speed = 50,
		.image = ASSET_TYPE_POWERUPBOMB
	},
	{ // SHIELD
		.speed = 50,
		.image = ASSET_TYPE_POWERUPSHIELD
	},
	{ // RAPIDFIRE
		.speed = 50,
		.image = ASSET_TYPE_POWERUPRAPIDFIRE
	},
};

void powerup_init(powerup_t *pu)
{
	pu->go.type = GAME_OBJECT_TYPE_POWERUP;
	pu->ro.parent = &pu->go;
}

void powerup_destroy(powerup_t *pu)
{
	powerup_diactivate(pu);
}

void powerup_activate(powerup_t *pu, powerup_type_t type,
		      int x, int y)
{
	const asset_info_t *ass_inf;
	asset_type_t ass_type;

	ass_type = _configs[type].image;

	ass_inf = wd_get_asset_info(ass_type);

	pu->ro.w = ass_inf->w;
	pu->ro.h = ass_inf->h;
	pu->ro.buff = wd_get_asset(ass_type);

	pu->type = type;
	pu->speed = _configs[type].speed;
	pu->collision_radius = ass_inf->w/2;

	pu->go.x = x;
	pu->go.y = y;

	pu->go.vy = pu->speed;

	pu->go.active = true;
}

void powerup_diactivate(powerup_t *pu)
{
	if (!pu->go.active)
		return;

	wd_not_using(_configs[pu->type].image);
	pu->go.active = false;
}

void powerup_update(powerup_t *pu, float dt)
{
	physics_update(&pu->go, dt);

	if (pu->go.y >= SCREEN_H + pu->ro.h)
		pu->go.active = false;
}
