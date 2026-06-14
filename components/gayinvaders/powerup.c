#include <math.h>
#include <stdio.h>
#include "powerup.h"
#include "gayinvaders.h"
#include "physics.h"
#include "wd.h"

typedef struct {
	int speed;
} powerup_type_conf_t;

static powerup_type_conf_t _configs[POWERUP_TYPE_CNT] = {
	{ // HEALTH
		.speed = 50,
	},
	{ // BOMB
		.speed = 50,
	},
};

static int _powerup_img_consumers[POWERUP_TYPE_CNT] = { };
static uint16_t *_powerup_img[POWERUP_TYPE_CNT] = {};

void powerup_init(powerup_t *pu)
{
	pu->go.type = GAME_OBJECT_TYPE_POWERUP;
	pu->ro.parent = &pu->go;
}

void powerup_destroy(powerup_t *pu)
{
	int i;

	for (i = 0; i < POWERUP_TYPE_CNT; ++i) {
		if (_powerup_img[i]) {
			_powerup_img_consumers[i] -= 1;
			if (_powerup_img_consumers[i] == 0) {
				gayinvaders_free(_powerup_img[i]);
				_powerup_img[i] = NULL;
			}
		}
	}
}

void powerup_activate(powerup_t *pu, powerup_type_t type,
		      int x, int y)
{
	const asset_info_t *ass_inf;
	asset_type_t ass_type;

	ass_type = ASSET_TYPE_POWERUPHEALTH + type;

	ass_inf = wd_get_asset_info(ass_type);

	if (!_powerup_img[type]) {
		_powerup_img[type] = gayinvaders_malloc(ass_inf->w*ass_inf->h*2);
		wd_read_asset(ass_type, _powerup_img[type], 0, 0, ass_inf->w, ass_inf->h);
		_powerup_img_consumers[type] += 1;
	}

	pu->ro.w = ass_inf->w;
	pu->ro.h = ass_inf->h;
	pu->ro.buff = _powerup_img[type];

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
	pu->go.active = false;
}

void powerup_update(powerup_t *pu, float dt)
{
	physics_update(&pu->go, dt);

	if (pu->go.y >= SCREEN_H + pu->ro.h)
		pu->go.active = false;
}


