#include <stdio.h>
#include <string.h>
#include "player.h"
#include "bullet.h"
#include "gayinvaders.h"
#include "physics.h"
#include "timers.h"
#include "wd.h"

#define SPEEDX 200

#define NORMAL_SHOOT_INTERVAL 500

#define RAPIDFIRE_SHOOT_INTERVAL 100

void player_init(player_t *p, int x, int y)
{
	const asset_info_t *ass_inf;

	/* Player init */

	memset(p, 0, sizeof(player_t));

	p->go.type = GAME_OBJECT_TYPE_PLAYER;

	// Rendering
	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);
	p->ro.buff = wd_get_asset(ASSET_TYPE_PLAYER);

	p->ro.parent = &p->go;
	p->ro.w = ass_inf->w;
	p->ro.h = ass_inf->h;

	// Position
	p->go.x = x;
	p->go.y = y;
	p->go.vx = 0;
	p->go.vy = 0.0;
	p->go.ax = 0.0;
	p->go.ay = 0.0;

	p->go.resistance = 2.0f;

	// Player specific
	p->health = 5;
	p->collision_radius = p->ro.w / 2;

	p->prev_shot_t = gayinvaders_get_ms();

	p->shield_up = false;
	p->shield.go.active = false;
	p->shield.ro.parent = &p->shield.go;

	p->go.active = true;
}

void player_destroy(player_t *p)
{
	wd_not_using(ASSET_TYPE_PLAYER);
}

void player_update(player_t *p, float dt,
		   bullet_t *bullets, int bullets_cnt)
{
	if (p->dead)
		return;

	physics_update(&p->go, dt);
	if (p->health <= 0)
		p->dead = true;

	if (p->go.x - ((float)p->ro.w/2) <= 0)
		p->go.x = (float)p->ro.w/2;
	else if (p->go.x + ((float)p->ro.w/2) >= SCREEN_W)
		p->go.x = SCREEN_W - ((float)p->ro.w/2);

	if (p->shield_up) {
		p->shield.go.x = p->go.x;
		p->shield.go.y = p->go.y - p->ro.h;
	}

	if (p->rapidfire_on) {
		size_t t = gayinvaders_get_ms();
		int i;

		if (t - p->prev_shot_t < RAPIDFIRE_SHOOT_INTERVAL)
			return;
		p->prev_shot_t = t;

		for (i = 0; i < bullets_cnt; ++i) {
			bullet_t *b = &bullets[i];
			int ret;

			if (b->go.active)
				continue;

			ret = bullet_activate(b, BULLET_TYPE_NORMAL, p->go.x, p->go.y,
					      p->go.x, p->go.y-10);
			if (ret)
				printf("Bullet alloc failed!\n");
			break;
		}
	}
}
void player_render(player_t *p)
{
	renderer_render(&p->ro);
	renderer_render(&p->shield.ro);
}

void player_go_stop(player_t *p)
{
	p->go.ax = 0;
}

void player_go_left(player_t *p)
{
	p->go.ax = -SPEEDX;
}

void player_go_right(player_t *p)
{
	p->go.ax = SPEEDX;
}

void player_fire(player_t *p, bullet_type_t bullet_type,
		 bullet_t *bullets, int bullets_cnt)
{
	size_t current_t;
	int i;

	if (p->dead)
		return;

	// Is it a player bullet
	if (!bullet_is_players(bullet_type))
		return;

	if (p->rapidfire_on)
		return;

	if (bullet_type == BULLET_TYPE_NORMAL) {
		current_t = gayinvaders_get_ms();
		if (current_t - p->prev_shot_t < NORMAL_SHOOT_INTERVAL)
			return;
		p->prev_shot_t = current_t;
	} else if (bullet_type == BULLET_TYPE_BOMB) {
		if (!p->has_bomb)
			return;
		p->has_bomb = false;
	}

	for (i = 0; i < bullets_cnt; ++i) {
		bullet_t *b = &bullets[i];
		int ret;

		if (b->go.active)
			continue;

		ret = bullet_activate(b, bullet_type, p->go.x, p->go.y,
				      p->go.x, p->go.y-10);
		if (ret)
			printf("Bullet alloc failed!\n");
		break;
	}
}

void player_damage(player_t *p, int amount)
{
	if (p->shield_up)
		return;

	p->health -= amount;

	if (p->health < 0)
		p->health = 0;
}

static void _shield_timeout(void *data)
{
	player_t *p = data;

	wd_not_using(ASSET_TYPE_SHIELD);
	p->shield.go.active = false;

	p->shield_up = false;
}

void player_shield_up(player_t *p)
{
	const asset_info_t *ass_inf = wd_get_asset_info(ASSET_TYPE_SHIELD);

	if (!p->has_shield)
		return;

	if (p->shield_up)
		timers_stop(p->shield_timer);

	p->shield_timer = timers_start(10000, false, p, _shield_timeout);

	p->shield.go.active = true;
	p->shield.ro.buff = wd_get_asset(ASSET_TYPE_SHIELD);
	p->shield.ro.w = ass_inf->w;
	p->shield.ro.h = ass_inf->h;

	p->shield_up = true;

	p->has_shield = false;
}

static void _rapid_fire_off(void* data)
{
	player_t *p = data;

	p->rapidfire_on = false;
}

void player_rapidfire_on(player_t *p)
{
	if (!p->has_rapidfire)
		return;


	p->rapidfire_on = true;

	timers_start(4000, false, p, _rapid_fire_off);

	p->has_rapidfire = false;
}
