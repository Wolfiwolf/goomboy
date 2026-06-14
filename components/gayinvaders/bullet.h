#ifndef BULLET_H_
#define BULLET_H_

#include "gameobject.h"
#include "renderer.h"

typedef enum {
	BULLET_TYPE_NORMAL = 0,
	BULLET_TYPE_BOMB,
	BULLET_TYPE_ENEMY_NORMAL,
	BULLET_TYPE_CNT,
} bullet_type_t;

#define BULLET_PLAYER_SPECIAL_START BULLET_TYPE_BOMB
#define BULLET_PLAYER_SPECIAL_CNT   (BULLET_TYPE_ENEMY_NORMAL-BULLET_TYPE_BOMB)

typedef struct {
	game_object_t go;
	render_obj_t ro;

	bullet_type_t type;
	int damage;
	int speed;
	bool has_hit;
	int collision_radius;

	/* Flag so players bullets don't damage him */
	bool damage_player;
} bullet_t;

void bullet_init(bullet_t *b);

void bullet_destroy(bullet_t *b);

void bullet_activate(bullet_t *b, bullet_type_t type,
		     int x, int y, int targetx, int targety,
		     bool damage_player);

void bullet_diactivate(bullet_t *b);

void bullet_update(bullet_t *b, float dt);

void bullet_hit(bullet_t *b);

#endif
