#include <math.h>
#include <stdio.h>
#include "bullet.h"
#include "gayinvaders.h"
#include "physics.h"
#include "timers.h"
#include "wd.h"

typedef struct {
	int speed;
	int acceleration;
	int damage;
} bullet_type_conf_t;

static bullet_type_conf_t _configs[BULLET_TYPE_CNT] = {
	{ // NORMAL
		.speed = 250,
		.acceleration = 0,
		.damage = 1,
	},
	{ // BOMB
		.speed = 0,
		.acceleration = 250,
		.damage = 3,
	},
	{ // ENEMY NORMAL
		.speed = 250,
		.acceleration = 0,
		.damage = 1,
	}
};

void bullet_init(bullet_t *b)
{

	b->go.type = GAME_OBJECT_TYPE_BULLET;
	b->ro.parent = &b->go;
}

void bullet_destroy(bullet_t *b)
{
	// Empty
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
		     int x, int y, int targetx, int targety)
{
	const asset_info_t *ass_inf;
	asset_type_t ass_type;
	float dirx, diry;

	// Load hit asset
	ass_type = ASSET_TYPE_BULLETNORMALHIT+type;
	ass_inf = wd_get_asset_info(ass_type);

	// Load asset
	ass_type = ASSET_TYPE_BULLETNORMAL+type;
	ass_inf = wd_get_asset_info(ass_type);

	// Setup
	_get_dir(x, y, targetx, targety, &dirx, &diry);

	// Render
	b->ro.w = ass_inf->w;
	b->ro.h = ass_inf->h;
	b->ro.buff = wd_get_asset(ASSET_TYPE_BULLETNORMAL+type);

	// Bullet specifics
	b->type = type;
	b->speed = _configs[type].speed;
	b->damage = _configs[type].damage;
	b->has_hit = false;
	b->collision_radius = ass_inf->w/2;

	// Position
	b->go.x = x;
	b->go.y = y;

	b->go.vx = dirx * b->speed;
	b->go.vy = diry * b->speed;

	b->go.ax = dirx * _configs[type].acceleration;
	b->go.ay = diry * _configs[type].acceleration;

	b->go.active = true;
}

void bullet_update(bullet_t *b, float dt)
{
	if (!b->go.active || b->has_hit)
		return;

	physics_update(&b->go, dt);

	/* Disable bullet if out of screen */
	if (b->go.y < 0 - b->ro.h)
		bullet_diactivate(b);

	if (b->go.y >= SCREEN_H + b->ro.h)
		bullet_diactivate(b);
}

void bullet_diactivate(bullet_t *b)
{
	wd_not_using(ASSET_TYPE_BULLETNORMAL+b->type);

	if (b->has_hit)
		wd_not_using(ASSET_TYPE_BULLETNORMALHIT+b->type);
	b->go.active = false;
}

static void _diactivate(void *data)
{
	bullet_t *b = data;

	bullet_diactivate(b);
}

void bullet_hit(bullet_t *b)
{
	const asset_info_t *ass_inf = wd_get_asset_info(ASSET_TYPE_BULLETNORMALHIT+b->type);

	if (b->has_hit)
		return;

	b->has_hit = true;

	b->ro.w = ass_inf->w;
	b->ro.h = ass_inf->h;
	b->ro.buff = wd_get_asset(ASSET_TYPE_BULLETNORMALHIT+b->type);

	timers_start(1000, false, b, _diactivate);
}

bool bullet_is_players(bullet_type_t btype)
{
	return btype >= BULLET_TYPE_NORMAL &&
	       btype < BULLET_TYPE_ENEMY_NORMAL;
}
