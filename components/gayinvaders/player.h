#ifndef PLAYER_H_
#define PLAYER_H_

#include "bullet.h"
#include "gameobject.h"
#include "renderer.h"

typedef struct {
	game_object_t go;
	render_obj_t ro;

	bool dead;
	int health;

	int collision_radius;

	bool ammo[BULLET_PLAYER_SPECIAL_CNT];
} player_t;

void player_init(player_t *p, int x, int y);

void player_destroy(player_t *p);

void player_update(player_t *p, float dt);

void player_go_stop(player_t *p);

void player_go_left(player_t *p);

void player_go_right(player_t *p);

void player_fire(player_t *p, bullet_type_t bullet_type,
		 bullet_t *bullets, int bullets_cnt);

#endif
