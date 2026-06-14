#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enemy.h"
#include "bullet.h"
#include "gayinvaders.h"
#include "physics.h"
#include "timers.h"
#include "wd.h"

typedef struct {
	int health;
	int speedx;
	int speedy;
	int turn_distance;
	int shoot_countdown;
	bullet_type_t bullet_type;
} enemy_config_t;

static enemy_config_t _configs[ENEMY_TYPE_CNT] = {
	{
		.health = 1,
		.speedx = 50,
		.speedy = 15,
		.turn_distance = 0,
		.shoot_countdown = 2500,
		.bullet_type = BULLET_TYPE_ENEMY_NORMAL,
	}
};

static uint16_t *_enemy_easy_images[ENEMY_TYPE_CNT][ENEMY_IMG_CNT] = {};

static void _load_assets(enemy_type_t type)
{
	const asset_info_t *ass_inf;
	int i;

	/* Images already loaded */
	if (_enemy_easy_images[type][0])
		return;

	for (i = 0; i < ENEMY_IMG_CNT; ++i) {
		ass_inf = wd_get_asset_info(ASSET_TYPE_ENEMYEASYIDLE+i);
		_enemy_easy_images[type][i] = gayinvaders_malloc(ass_inf->w*ass_inf->h*2);

		switch (type) {
		case ENEMY_TYPE_EASY:
			wd_read_asset(ASSET_TYPE_ENEMYEASYIDLE+i, _enemy_easy_images[type][i], 0, 0, ass_inf->w, ass_inf->h);
			break;
		case ENEMY_TYPE_MID:
		case ENEMY_TYPE_HARD:
		case ENEMY_TYPE_BOSS:
		default:
			gayinvaders_free(_enemy_easy_images[type][i]);
			break;
		}
	}
}

static void _unload_assets(enemy_type_t type)
{
	int i;

	for (i = 0; i < ENEMY_IMG_CNT; ++i) {
		gayinvaders_free(_enemy_easy_images[type][i]);
		_enemy_easy_images[type][i] = NULL;
	}
}

void enemy_init(enemy_t *e)
{
	int i;

	memset(e, 0, sizeof(enemy_t));
	
	e->go.type = GAME_OBJECT_TYPE_ENEMY;

	for (i = 0; i < ENEMY_IMG_CNT; ++i)
		e->images[i].parent = &e->go;
}

void enemy_destroy(enemy_t *e)
{

}

static void _change_img_to_idle(void *data)
{
	enemy_t *e = data;

	if (!e->dead)
		e->active_image = ENEMY_IMG_IDLE;
}

void enemy_update(enemy_t *e, float dt,
		  bullet_t *bullets, int bullets_cnt,
		  game_object_t *player_go)
{
	int speedx = _configs[e->enemy_type].speedx;
	int i;

	if (!e->go.active)
		return;

	if (e->dead)
		goto basic_update;

	if (e->turn_distance != 0) {
		if (e->go.x - e->turn_center >= e->turn_distance)
			e->go.vx = -speedx;
		if (e->go.x - e->turn_center <= -e->turn_distance)
			e->go.vx = speedx;
		if (e->go.x+(float)e->images[0].w/2 >= SCREEN_W)
			e->go.vx = -speedx;
		if (e->go.x-(float)e->images[0].w/2 <= 0)
			e->go.vx = speedx;
	}


	e->shoot_countdown -= dt*1000;
	if (e->shoot_countdown <= 0) {
		for (i = 0; i < bullets_cnt; ++i) {
			bullet_t *b = &bullets[i];

			if (b->go.active)
				continue;

			bullet_activate(b, _configs[e->enemy_type].bullet_type, e->go.x, e->go.y, player_go->x, player_go->y, true);
			break;
		}
		e->shoot_countdown = _configs[e->enemy_type].shoot_countdown;
		e->active_image = ENEMY_IMG_SHOOT;
		timers_start(500, false, e, _change_img_to_idle);
	}

basic_update:
	physics_update(&e->go, dt);
}

void enemy_activate(enemy_t *e, enemy_type_t type, int x, int y)
{
	int tdist = _configs[type].turn_distance;
	const asset_info_t *ass_inf;
	int i;

	ass_inf = wd_get_asset_info(ASSET_TYPE_ENEMYEASYIDLE);

	_load_assets(type);

	// Position
	
	if (x == ENEMY_X_POS_RANDOM) {
		x = rand() % (SCREEN_W - (tdist*2));
		x += tdist;
	}

	e->go.y = y;
	e->go.vy = _configs[type].speedy;

	if (e->turn_distance) {
		e->go.x = x + ((rand() % (_configs[type].turn_distance*2))-_configs[type].turn_distance);
		e->go.vx = _configs[type].speedx;
	} else {
		e->go.x = x;
		e->go.vx = 0;
	}

	// Enemy specific
	e->enemy_type = type;
	e->turn_distance = _configs[type].turn_distance;
	e->turn_center = x;
	e->shoot_countdown = _configs[type].shoot_countdown;
	e->dead = false;

	// Rendering
	for (i = 0; i < ENEMY_IMG_CNT; ++i) {
		e->images[i].w = ass_inf->w;
		e->images[i].h = ass_inf->h;
		e->images[i].buff = _enemy_easy_images[e->enemy_type][i];
		e->collision_radius = ass_inf->w/2;
	}

	e->active_image = ENEMY_IMG_IDLE;

	e->go.active = true;
}

void enemy_diactivate(enemy_t *e)
{
	e->go.active = false;
}

static void _kill_enemy(void *enemy)
{
	enemy_diactivate(enemy);
}

void enemy_damage(enemy_t *e, int damage)
{
	e->health -= damage;

	if (e->health <= 0)
		enemy_kill(e);
}

void enemy_kill(enemy_t *e)
{
	e->go.vx *= 0.3;
	e->go.vy *= 0.3;
	e->active_image = ENEMY_IMG_PAIN;
	e->dead = true;

	timers_start(1000, false, e, _kill_enemy);
}
