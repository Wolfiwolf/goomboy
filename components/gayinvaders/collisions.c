#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "collisions.h"
#include "bullet.h"
#include "enemy.h"
#include "gameobject.h"
#include "player.h"
#include "powerup.h"

static void (*_on_collision_handler)(void *obj1, game_object_type_t type1, void *obj2, game_object_type_t type2) = NULL;

static player_t *_player;
static bullet_t *_bullets;
static int _bullets_cnt;
static enemy_t *_enemies;
static int _enemies_cnt;
static powerup_t *_powerups;
static int _powerups_cnt;

void collisions_init(player_t *player,
		     bullet_t *bullets, int bullets_cnt,
		     enemy_t *enemies, int enemies_cnt,
		     powerup_t *powerups, int powerups_cnt,
		     void (*on_collision)(void *obj1, game_object_type_t type1, void *obj2, game_object_type_t type2))
{
	_player = player;

	_bullets = bullets;
	_enemies = enemies;
	_powerups = powerups;

	_bullets_cnt = bullets_cnt;
	_enemies_cnt = enemies_cnt;
	_powerups_cnt = powerups_cnt;

	_on_collision_handler = on_collision;
}

static float _get_dist(const game_object_t *go1, const game_object_t *go2)
{
	float distx = go1->x - go2->x;
	float disty = go1->y - go2->y;
	return sqrtf(distx*distx + disty*disty);
}

void collision_update(void)
{
	int i,j;

	for (i = 0; i < _bullets_cnt; ++i) {
		bullet_t *b = &_bullets[i];

		if (!b->go.active)
			continue;

		if (_player->go.active && b->damage_player) {
			float dist = _get_dist(&_player->go, &b->go);

			if (dist < _player->collision_radius)
				_on_collision_handler(_player, _player->go.type, b, b->go.type);
		}

		for (j = 0; j < _powerups_cnt; ++j) {
			powerup_t *pu = &_powerups[j];
			float dist;

			if (!pu->go.active)
				continue;

			dist = _get_dist(&_player->go, &pu->go);

			if (dist < _player->collision_radius+pu->collision_radius) {
				_on_collision_handler(_player, _player->go.type,
						      pu, pu->go.type);
				break;
			}
		}

		if (b->damage_player)
			continue;

		for (j = 0; j < _enemies_cnt; ++j) {
			enemy_t *e = &_enemies[j];
			float dist;

			if (!e->go.active)
				continue;

			dist = _get_dist(&e->go, &b->go);

			if (dist < e->collision_radius) {
				_on_collision_handler(e, e->go.type, b, b->go.type);
				break;
			}
		}
	}

}
