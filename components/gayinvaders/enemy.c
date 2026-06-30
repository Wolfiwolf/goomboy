#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enemy.h"
#include "bullet.h"
#include "gayinvaders.h"
#include "physics.h"
#include "renderer.h"
#include "timers.h"
#include "wd.h"

typedef struct {
	int health;
	int speedx;
	int speedy;
	int turn_distance;
	int shoot_countdown;
	bullet_type_t bullet_type;
	asset_type_t images[ENEMY_IMG_CNT];
} enemy_config_t;

static enemy_config_t _configs[ENEMY_TYPE_CNT] = {
	{ // EASY
		.health = 2,
		.speedx = 0,
		.speedy = 15,
		.turn_distance = 0,
		.shoot_countdown = 2500,
		.bullet_type = BULLET_TYPE_ENEMY_NORMAL,
		.images = {
			ASSET_TYPE_ENEMYEASYIDLE,
			ASSET_TYPE_ENEMYEASYSHOOT,
			ASSET_TYPE_ENEMYEASYPAIN,
		}
	},
	{ // MOVING
		.health = 3,
		.speedx = 50,
		.speedy = 15,
		.turn_distance = 100,
		.shoot_countdown = 2500,
		.bullet_type = BULLET_TYPE_ENEMY_NORMAL,
		.images = {
			ASSET_TYPE_ENEMYMOVINGIDLE,
			ASSET_TYPE_ENEMYMOVINGSHOOT,
			ASSET_TYPE_ENEMYMOVINGPAIN,
		}
	},
	{ // FAST
		.health = 1,
		.speedx = 0,
		.speedy = 70,
		.turn_distance = 0,
		.shoot_countdown = 0,
		.bullet_type = BULLET_TYPE_ENEMY_NORMAL,
		.images = {
			ASSET_TYPE_ENEMYFASTIDLE,
			ASSET_TYPE_ENEMYFASTSHOOT,
			ASSET_TYPE_ENEMYFASTPAIN,
		}
	},
	{ // TANK
		.health = ENEMY_MAX_HEALTH,
		.speedx = 50,
		.speedy = 15,
		.turn_distance = 0,
		.shoot_countdown = 1000,
		.bullet_type = BULLET_TYPE_ENEMY_NORMAL,
		.images = {
			ASSET_TYPE_ENEMYTANKIDLE,
			ASSET_TYPE_ENEMYTANKSHOOT,
			ASSET_TYPE_ENEMYTANKPAIN,
		}
	}
};

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
	enemy_diactivate(e);
}

static void _change_img_to_idle(void *data)
{
	enemy_t *e = data;

	if (!e->dead)
		e->active_image = ENEMY_IMG_IDLE;
}

void enemy_update(enemy_t *e, float dt,
		  bullet_t *bullets, int bullets_cnt,
		  player_t *player)
{
	game_object_t *player_go = &player->go;
	int start_x;
	int i;

	if (!e->go.active)
		return;

	if (e->dead)
		goto basic_update;

	if (e->turn_distance != 0) {
		int speedx = _configs[e->enemy_type].speedx;
		if (e->go.x - e->turn_center >= e->turn_distance)
			e->go.vx = -speedx;
		if (e->go.x - e->turn_center <= -e->turn_distance)
			e->go.vx = speedx;
		if (e->go.x+(float)e->images[0].w/2 >= SCREEN_W)
			e->go.vx = -speedx;
		if (e->go.x-(float)e->images[0].w/2 <= 0)
			e->go.vx = speedx;
	}

	if (e->shoot_countdown) {
		e->shoot_countdown -= dt*1000;
		if (e->shoot_countdown <= 0) {
			for (i = 0; i < bullets_cnt; ++i) {
				bullet_t *b = &bullets[i];

				if (b->go.active)
					continue;

				bullet_activate(b, _configs[e->enemy_type].bullet_type, e->go.x, e->go.y, player_go->x, player_go->y);
				break;
			}
			e->shoot_countdown = _configs[e->enemy_type].shoot_countdown;
			e->active_image = ENEMY_IMG_SHOOT;
			timers_start(500, false, e, _change_img_to_idle);
		}
	}

basic_update:
	physics_update(&e->go, dt);

	start_x = e->go.x - (float)e->images[e->active_image].w/2;
	for (i = 0; i < ENEMY_MAX_HEALTH; ++i) {
		enemy_health_block_t *ehb = &e->health_blocks[i];

		if (i >= e->health) {
			e->health_blocks[i].go.active = false;
			continue;
		}
		e->health_blocks[i].go.active = true;

		ehb->go.x = start_x + i*ehb->ro.w;
		ehb->go.y = e->go.y - (float)(e->images[e->active_image].h*3)/4;
	}
}

void enemy_render(enemy_t *e)
{
	int i;

	if (!e->go.active)
		return;

	renderer_render(&e->images[e->active_image]);

	for (i = 0; i < ENEMY_MAX_HEALTH; ++i) {
		enemy_health_block_t *ehb = &e->health_blocks[i];

		renderer_render(&ehb->ro);
	}
}

int enemy_activate(enemy_t *e, enemy_type_t type, int x, int y)
{
	int tdist = _configs[type].turn_distance;
	const asset_info_t *ass_inf;
	int i;

	ass_inf = wd_get_asset_info(_configs[type].images[ENEMY_IMG_IDLE]);

	// Position
	
	if (x == ENEMY_X_POS_RANDOM) {
		x = rand() % (SCREEN_W - (tdist*2) - (ass_inf->w));
		x += tdist + (ass_inf->w/2);
	}

	e->go.y = y;
	e->go.vy = _configs[type].speedy;

	if (tdist) {
		e->go.x = x + ((rand() % (tdist*2)) - tdist);
		e->go.vx = _configs[type].speedx;
	} else {
		e->go.x = x;
		e->go.vx = 0;
	}

	// Enemy specific
	e->enemy_type = type;
	e->turn_distance = tdist;
	e->turn_center = x;
	e->shoot_countdown = _configs[type].shoot_countdown;
	e->health = _configs[type].health;
	e->dead = false;

	// Rendering
	for (i = 0; i < ENEMY_IMG_CNT; ++i) {
		e->images[i].w = ass_inf->w;
		e->images[i].h = ass_inf->h;
		e->images[i].buff = wd_get_asset(_configs[type].images[i]);
		if (!e->images[i].buff) {
			int j;
			for (j = i; j >= 0; --j)
				wd_not_using(_configs[type].images[j]);
			return -ENOMEM;
		}
		e->collision_radius = ass_inf->w/2;
	}

	e->active_image = ENEMY_IMG_IDLE;

	ass_inf = wd_get_asset_info(ASSET_TYPE_HEALTHBARBLOCK);

	for (i = 0; i < ENEMY_MAX_HEALTH; ++i) {
		e->health_blocks[i].go.x = e->go.x;
		e->health_blocks[i].go.y = e->go.y;

		e->health_blocks[i].ro.parent = &e->health_blocks[i].go;
		e->health_blocks[i].ro.buff = wd_get_asset(ASSET_TYPE_HEALTHBARBLOCK);
		e->health_blocks[i].ro.w = ass_inf->w;
		e->health_blocks[i].ro.h = ass_inf->h;
	}

	e->go.active = true;

	return 0;
}

void enemy_diactivate(enemy_t *e)
{
	int i;

	if (!e->go.active)
		return;

	e->go.active = false;
	for (i = 0; i < ENEMY_IMG_CNT; ++i)
		wd_not_using(_configs[e->enemy_type].images[i]);

	for (i = 0; i < ENEMY_MAX_HEALTH; ++i) {
		e->health_blocks[i].go.active = false;
		wd_not_using(ASSET_TYPE_HEALTHBARBLOCK);
	}
}

static void _kill_enemy(void *enemy)
{
	enemy_diactivate(enemy);
}

void enemy_damage(enemy_t *e, int damage)
{
	e->health -= damage;

	if (e->health <= 0) {
		enemy_kill(e);
	} else {
		e->active_image = ENEMY_IMG_PAIN;
		timers_start(250, false, e, _change_img_to_idle);
	}
}

void enemy_kill(enemy_t *e)
{
	e->go.vx *= 0.3;
	e->go.vy *= 0.3;
	e->active_image = ENEMY_IMG_PAIN;
	e->dead = true;

	timers_start(1000, false, e, _kill_enemy);
}
