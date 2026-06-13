#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enemy.h"
#include "gayinvaders.h"
#include "wd.h"

#define ENEMY_EASY_SPEED_X  50
#define ENEMY_EASY_SPEED_Y  25
#define ENEMY_TURN_DISTANCE 100

static uint16_t *_enemy_easy_images[ENEMY_TYPE_CNT][ENEMY_IMG_CNT] = {};

static void _load_assets(enemy_type_t type)
{
	const asset_info_t *ass_inf;
	int i;

	/* Images already loaded */
	if (_enemy_easy_images[type][0])
		return;

	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);

	for (i = 0; i < ENEMY_IMG_CNT; ++i) {
		_enemy_easy_images[type][i] = gayinvaders_malloc(ass_inf->size);

		switch (type) {
		case ENEMY_TYPE_EASY:
			wd_read_asset(ASSET_TYPE_PLAYER, _enemy_easy_images[type][i], 0, 0, ass_inf->w, ass_inf->h);
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

void enemy_update(enemy_t *e, float dt)
{
	int speedx;

	switch (e->enemy_type) {
	case ENEMY_TYPE_EASY:
		speedx = ENEMY_EASY_SPEED_X;
		break;
	default:
		break;
	}

	if ((int)(e->go.x - e->turn_center) >= e->turn_distance) {
		e->go.vx = -speedx;
	}
	if ((int)(e->go.x - e->turn_center) <= -e->turn_distance) {
		e->go.vx = speedx;
	}
}

static void _enemy_activate_easy(enemy_t *e)
{
	const asset_info_t *ass_inf;
	int i;

	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);

	e->go.vx = ENEMY_EASY_SPEED_X;
	e->go.vy = ENEMY_EASY_SPEED_Y;
	e->turn_distance = ENEMY_TURN_DISTANCE;
	e->turn_center = e->go.x;

	for (i = 0; i < ENEMY_IMG_CNT; ++i) {
		e->images[i].w = ass_inf->w;
		e->images[i].h = ass_inf->h;
		e->images[i].buff = _enemy_easy_images[ENEMY_TYPE_EASY][i];
	}
}

void enemy_activate(enemy_t *e, enemy_type_t type, int x, int y)
{
	_load_assets(type);

	e->enemy_type = type;

	e->go.x = x - (rand() % (ENEMY_TURN_DISTANCE*2));
	e->go.y = y;
	e->turn_center = x;

	_enemy_activate_easy(e);

	e->go.active = true;
}

void enemy_diactivate(enemy_t *e)
{
	e->go.active = false;
}
