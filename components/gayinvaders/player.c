#include <stdio.h>
#include <string.h>
#include "player.h"
#include "bullet.h"
#include "gayinvaders.h"
#include "physics.h"
#include "wd.h"

#define SPEEDX 50

void player_init(player_t *p, int x, int y)
{
	const asset_info_t *ass_inf;

	/* Player init */

	memset(p, 0, sizeof(player_t));

	p->go.type = GAME_OBJECT_TYPE_PLAYER;

	// Rendering
	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);
	p->ro.buff = gayinvaders_malloc(ass_inf->h*ass_inf->w*2);
	wd_read_asset(ASSET_TYPE_PLAYER, p->ro.buff, 0, 0, ass_inf->w, ass_inf->h);
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

	// Player specific
	p->health = 5;
	p->collision_radius = p->ro.w / 2;
	p->go.active = true;
}

void player_destroy(player_t *p)
{
	gayinvaders_free(p->ro.buff);
}

void player_update(player_t *p, float dt)
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
}

void player_go_stop(player_t *p)
{
	p->go.vx = 0;
}

void player_go_left(player_t *p)
{
	p->go.vx = -SPEEDX;
}

void player_go_right(player_t *p)
{
	p->go.vx = SPEEDX;
}

void player_fire(player_t *p, bullet_type_t bullet_type,
		 bullet_t *bullets, int bullets_cnt)
{
	int i;

	if (p->dead)
		return;

	// Is it a player bullet
	if (!bullet_is_players(bullet_type))
		return;

	for (i = 0; i < bullets_cnt; ++i) {
		bullet_t *b = &bullets[i];

		if (b->go.active)
			continue;

		bullet_activate(b, bullet_type, p->go.x, p->go.y,
				p->go.x, p->go.y-10);

		if (bullet_type != BULLET_TYPE_NORMAL)
			p->ammo[bullet_type-BULLET_PLAYER_SPECIAL_START] -= 1;
		break;
	}
}
