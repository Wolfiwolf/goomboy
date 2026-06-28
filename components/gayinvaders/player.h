#ifndef PLAYER_H_
#define PLAYER_H_

#include "bullet.h"
#include "gameobject.h"
#include "renderer.h"
#include "timers.h"

#define PLAYER_MAX_HEALTH 5

typedef struct {
	game_object_t go;
	render_obj_t ro;
} player_shield_t;

typedef struct {
	game_object_t go;
	render_obj_t ro;

	player_shield_t shield;

	bool dead;
	int health;

	int collision_radius;

	bool has_bomb;
	bool has_shield;
	bool has_rapidfire;

	size_t prev_shot_t;

	bool shield_up;
	bool rapidfire_on;

	timer_handle_t *shield_timer;
} player_t;

void player_init(player_t *p, int x, int y);

void player_destroy(player_t *p);

void player_update(player_t *p, float dt,
		   bullet_t *bullets, int bullets_cnt);

void player_render(player_t *p);

void player_go_stop(player_t *p);

void player_go_left(player_t *p);

void player_go_right(player_t *p);

void player_fire(player_t *p, bullet_type_t bullet_type,
		 bullet_t *bullets, int bullets_cnt);

void player_damage(player_t *p, int amount);

void player_shield_up(player_t *p);

void player_rapidfire_on(player_t *p);

#endif
