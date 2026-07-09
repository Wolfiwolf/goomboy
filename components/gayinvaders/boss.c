#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boss.h"
#include "gayinvaders.h"
#include "physics.h"
#include "renderer.h"
#include "wd.h"

#define BOSS_HEALTH 50
#define BOSS_START_X ((float)SCREEN_W/2)
#define BOSS_START_Y 60

#define PHAZE_IDLE_SPEEDY 20

#define PHAZE_0_SPEEDX      100
#define PHAZE_0_SHOOT_DELAY 500

#define PHAZE_1_TELEPORT_DELAY 2000

static const size_t _phaze_durations[BOSS_PHAZE_CNT] = {
	5000, // IDLE
	7500, // PHAZE_0
	7500, // PHAZE_1
};

void boss_init(boss_t *b)
{
	const asset_info_t *ass_inf;
	size_t t;
	int i;

	memset(b, 0, sizeof(boss_t));
	
	b->go.type = GAME_OBJECT_TYPE_BOSS;

	ass_inf = wd_get_asset_info(ASSET_TYPE_BOSSIDLE);

	b->image.parent = &b->go;
	b->image.buff = wd_get_asset(ASSET_TYPE_BOSSIDLE);
	b->image.w = ass_inf->w;
	b->image.h = ass_inf->h;

	t = gayinvaders_get_ms();

	b->go.x = BOSS_START_X;
	b->go.y = BOSS_START_Y;

	b->prev_phaze_switch_t = t;
	b->prev_shoot_t = t;

	b->health = BOSS_HEALTH;
	b->collision_radius = ass_inf->h/2;

	for (i = 0; i < BOSS_HEALTH_BLOCKS_CNT; ++i) {
		ass_inf = wd_get_asset_info(ASSET_TYPE_BOSSHEALTH);
		boss_health_block_t *hb = &b->health_blocks[i];

		hb->ro.parent = &hb->go;
		hb->ro.buff = wd_get_asset(ASSET_TYPE_BOSSHEALTH);
		hb->ro.w = ass_inf->w;
		hb->ro.h = ass_inf->h;

		hb->go.y = (float)ass_inf->h/2;
		hb->go.x = ((float)ass_inf->w/2) + i * ass_inf->w;
		hb->go.active = true;
	}

	b->go.active = true;
}

static void _shoot_normal(boss_t *b, bullet_t *bullets, int bullets_cnt,
			  player_t *player)
{
	int i;

	for (i = 0; i < bullets_cnt; ++i) {
		bullet_t *bull = &bullets[i];

		if (bull->go.active)
			continue;

		bullet_activate(bull, BULLET_TYPE_ENEMY_NORMAL, b->go.x, b->go.y, player->go.x, player->go.y);
		break;
	}
}

static void _shoot_circle(boss_t *b, bullet_t *bullets, int bullets_cnt)
{
	int bullet_index = 0;
	int i;

	for (i = 0; i < bullets_cnt; ++i) {
		bullet_t *bull = &bullets[i];
		int x, y;
		int dstx, dsty;

		if (bull->go.active)
			continue;

		x = b->go.x + b->image.w * cosf((bullet_index * 20)*(3.14/180.0));
		y = b->go.y + b->image.w * sinf((bullet_index * 20)*(3.14/180.0));

		dstx = b->go.x + 2*b->image.w * cosf(bullet_index * 20*(3.14/180.0));
		dsty = b->go.y + 2*b->image.w * sinf(bullet_index * 20*(3.14/180.0));

		bullet_activate(bull, BULLET_TYPE_ENEMY_NORMAL, x, y, dstx, dsty);

		bullet_index += 1;
		if (bullet_index > 9)
			break;
	}
}

static void _update_phaze_idle(boss_t *b)
{
	b->go.vx = 0;
	if (b->go.vy == 0)
		b->go.vy = PHAZE_IDLE_SPEEDY;

	if (b->go.y <= BOSS_START_Y-5)
		b->go.vy = PHAZE_IDLE_SPEEDY;
	else if (b->go.y >= BOSS_START_Y+5)
		b->go.vy = -PHAZE_IDLE_SPEEDY;
}

/**
 * Move from left to right and shoot
 * constantly normal ammo.
 */
static void _update_phaze_0(boss_t *b, bullet_t *bullets, int bullets_cnt,
			    player_t *player)
{
	size_t t;

	if (b->go.vx == 0)
		b->go.vx = PHAZE_0_SPEEDX;

	if (b->go.x <= 0)
		b->go.vx = PHAZE_0_SPEEDX;
	else if (b->go.x >= SCREEN_W)
		b->go.vx = -PHAZE_0_SPEEDX;

	t = gayinvaders_get_ms();
	if (t - b->prev_shoot_t >= PHAZE_0_SHOOT_DELAY) {
		_shoot_normal(b, bullets, bullets_cnt, player);
		b->prev_shoot_t = t;
	}
}

static void _phaze_1_teleport(boss_t *b, bullet_t *bullets, int bullets_cnt)
{
	int x = (rand() % (SCREEN_W - 2*b->image.w)) + b->image.w;
	int y = BOSS_START_Y + (rand() % (SCREEN_H_HALF-100));

	b->go.x = x;
	b->go.y = y;

	_shoot_circle(b, bullets, bullets_cnt);
}

/**
 * Teleport from place to place and shoot bullets
 * in half circle.
 */
static void _update_phaze_1(boss_t *b, bullet_t *bullets, int bullets_cnt)
{
	size_t t;

	b->go.vx = 0;
	if (b->go.vy == 0)
		b->go.vy = PHAZE_IDLE_SPEEDY;

	if (b->go.y <= BOSS_START_Y-5)
		b->go.vy = PHAZE_IDLE_SPEEDY;
	else if (b->go.y >= BOSS_START_Y+5)
		b->go.vy = -PHAZE_IDLE_SPEEDY;

	t = gayinvaders_get_ms();
	if (t - b->prev_teleport_t >= PHAZE_1_TELEPORT_DELAY) {
		_phaze_1_teleport(b, bullets, bullets_cnt);
		b->prev_teleport_t = t;
	}
}

void boss_update(boss_t *b, float dt, bullet_t *bullets, int bullets_cnt,
		 player_t *player)
{
	size_t t;


	physics_update(&b->go, dt);

	if (b->dead)
		return;

	t = gayinvaders_get_ms();

	if (t - b->prev_phaze_switch_t >= _phaze_durations[b->phaze]) {
		if (b->phaze != BOSS_PHAZE_IDLE) {
			b->go.y = BOSS_START_Y;
			b->prev_phaze = b->phaze;
			b->phaze = BOSS_PHAZE_IDLE;
		} else {
			b->go.vy = 0;
			b->go.y = BOSS_START_Y;
			b->phaze = b->prev_phaze+1;
			if (b->phaze == BOSS_PHAZE_CNT)
				b->phaze = BOSS_PHAZE_0;
		}

		b->prev_phaze_switch_t = t;
	}
	
	switch (b->phaze) {
	case BOSS_PHAZE_IDLE:
		_update_phaze_idle(b);
		break;
	case BOSS_PHAZE_0:
		_update_phaze_0(b, bullets, bullets_cnt, player);
		break;
	case BOSS_PHAZE_1:
		_update_phaze_1(b, bullets, bullets_cnt);
		break;
	default:
		break;
	}
}

void boss_render(boss_t *b)
{
	float health_percent;
	int i;

	if (!b->go.active)
		return;

	if (!b->dead) {
		health_percent = (float)b->health / BOSS_HEALTH;
		for (i = 0; i < BOSS_HEALTH_BLOCKS_CNT; ++i) {
			float percent = (float)i / BOSS_HEALTH_BLOCKS_CNT;

			if (percent <= health_percent)
				renderer_render(&b->health_blocks[i].ro);
		}
	}

	renderer_render(&b->image);
}

void boss_destroy(boss_t *b)
{
	int i;

	if (b->dead)
		wd_not_using(ASSET_TYPE_BOSSDEAD);
	else
		wd_not_using(ASSET_TYPE_BOSSIDLE);

	for (i = 0; i < BOSS_HEALTH_BLOCKS_CNT; ++i)
		wd_not_using(ASSET_TYPE_BOSSHEALTH);
}

static void _die(boss_t *b)
{
	b->dead = true;
	b->go.vx = 0;
	b->go.vy = 0;

	wd_not_using(ASSET_TYPE_BOSSIDLE);
	b->image.buff = wd_get_asset(ASSET_TYPE_BOSSDEAD);
}

void boss_damage(boss_t *b, int amount)
{
	if (b->dead)
		return;

	b->health -= amount;

	if (b->health < 0)
		b->health = 0;

	if (b->health == 0)
		_die(b);
}
