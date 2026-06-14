#include <math.h>
#include <stdio.h>
#include "bullet.h"
#include "gayinvaders.h"
#include "physics.h"
#include "wd.h"

typedef struct {
	int speed;
	int damage;
} bullet_type_conf_t;

static bullet_type_conf_t _configs[BULLET_TYPE_CNT] = {
	{ // NORMAL
		.speed = 250,
		.damage = 1,
	},
	{ // BOMB
		.speed = 150,
		.damage = 3,
	},
	{ // ENEMY NORMAL
		.speed = 250,
		.damage = 1,
	}
};


static int _bullet_img_consumers[BULLET_TYPE_CNT] = { };
static uint16_t *_bullet_img[BULLET_TYPE_CNT] = {};
static uint16_t *_bullet_hit_img[BULLET_TYPE_CNT] = {};

void bullet_init(bullet_t *b)
{

	b->go.type = GAME_OBJECT_TYPE_BULLET;
	b->ro.parent = &b->go;
}

void bullet_destroy(bullet_t *b)
{
	int i;

	for (i = 0; i < BULLET_TYPE_CNT; ++i) {
		if (_bullet_img[i]) {
			_bullet_img_consumers[i] -= 1;
			if (_bullet_img_consumers[i] == 0) {
				gayinvaders_free(_bullet_img[i]);
				_bullet_img[i] = NULL;
			}
		}
	}
}

static void _get_dir(float ox, float oy, float tx, float ty,
		    float *nx, float *ny)
{
	float dx = tx - ox;
	float dy = ty - oy;
	float len = sqrtf(dx * dx + dy * dy);

	if (len > 0.0f) {       // guard against origin == target
		*nx = dx / len;
		*ny = dy / len;
	} else {
		*nx = 0.0f;
		*ny = 0.0f;
	}
}

void bullet_activate(bullet_t *b, bullet_type_t type,
		     int x, int y, int targetx, int targety,
		     bool damage_player)
{
	const asset_info_t *ass_inf;
	asset_type_t ass_type;
	float dirx, diry;

	ass_type = ASSET_TYPE_BULLETNORMAL+type;

	ass_inf = wd_get_asset_info(ass_type);

	if (!_bullet_img[type]) {
		_bullet_img[type] = gayinvaders_malloc(ass_inf->w*ass_inf->h*2);
		wd_read_asset(ass_type, _bullet_img[type], 0, 0, ass_inf->w, ass_inf->h);
		_bullet_img_consumers[type] += 1;
	}

	_get_dir(x, y, targetx, targety, &dirx, &diry);

	b->ro.w = ass_inf->w;
	b->ro.h = ass_inf->h;
	b->ro.buff = _bullet_img[type];

	b->speed = _configs[type].speed;
	b->damage = _configs[type].damage;

	b->damage_player = damage_player;

	b->go.x = x;
	b->go.y = y;

	b->go.vx = dirx * b->speed;
	b->go.vy = diry * b->speed;

	b->go.active = true;
}

void bullet_diactivate(bullet_t *b)
{
	b->go.active = false;
}

void bullet_update(bullet_t *b, float dt)
{
	physics_update(&b->go, dt);

	/* Disable bullet if out of screen */
	if (b->go.y < 0 - b->ro.h)
		b->go.active = false;

	if (b->go.y >= SCREEN_H + b->ro.h)
		b->go.active = false;
}

void bullet_hit(bullet_t *b)
{
	/*
	asset_info_t ass_inf

	b->has_hit = true;

	b->ro.w = ass_inf->w;
	b->ro.h = ass_inf->h;
	b->ro.buff = _bullet_img[type];
	*/
}
