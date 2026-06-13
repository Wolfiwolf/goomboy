#ifndef ENEMY_H_
#define ENEMY_H_

#include "gameobject.h"
#include "renderer.h"

typedef enum {
	ENEMY_TYPE_EASY = 0,
	ENEMY_TYPE_MID,
	ENEMY_TYPE_HARD,
	ENEMY_TYPE_BOSS,
	ENEMY_TYPE_CNT,
} enemy_type_t;

typedef enum {
	ENEMY_IMG_LEFT = 0,
	ENEMY_IMG_SHOOT,
	ENEMY_IMG_PAIN,
	ENEMY_IMG_CNT,
} enemy_img_t;

typedef struct {
	enemy_type_t enemy_type;
	game_object_t go;
	render_obj_t images[ENEMY_IMG_CNT];
	int health;

	int turn_distance;
	int turn_center;
} enemy_t;

void enemy_init(enemy_t *e);

void enemy_update(enemy_t *e, float dt);

void enemy_activate(enemy_t *e, enemy_type_t type,
		    int x, int y);

void enemy_diactivate(enemy_t *e);

#endif
