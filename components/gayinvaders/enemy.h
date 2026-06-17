#ifndef ENEMY_H_
#define ENEMY_H_

#include "bullet.h"
#include "gameobject.h"
#include "renderer.h"

#define ENEMY_X_POS_RANDOM -1

#define ENEMY_MAX_HEALTH 5

typedef enum {
	ENEMY_TYPE_EASY = 0,
	ENEMY_TYPE_MOVING,
	ENEMY_TYPE_FAST,
	ENEMY_TYPE_TANK,
	ENEMY_TYPE_BOSS,
	ENEMY_TYPE_CNT,
} enemy_type_t;

typedef enum {
	ENEMY_IMG_IDLE = 0,
	ENEMY_IMG_SHOOT,
	ENEMY_IMG_PAIN,
	ENEMY_IMG_CNT,
} enemy_img_t;

typedef struct {
	game_object_t go;
	render_obj_t ro;
} enemy_health_block_t;

typedef struct {
	enemy_type_t enemy_type;
	game_object_t go;
	render_obj_t images[ENEMY_IMG_CNT];
	enemy_img_t active_image;

	enemy_health_block_t health_blocks[ENEMY_MAX_HEALTH];

	bool dead;

	int health;
	int collision_radius;

	int turn_distance;
	int turn_center;

	int shoot_countdown;
} enemy_t;

void enemy_init(enemy_t *e);

void enemy_destroy(enemy_t *e);

void enemy_update(enemy_t *e, float dt,
		  bullet_t *bullets, int bullets_cnt,
		  game_object_t *player_go);

void enemy_render(enemy_t *e);

void enemy_activate(enemy_t *e, enemy_type_t type,
		    int x, int y);

void enemy_diactivate(enemy_t *e);

void enemy_damage(enemy_t *e, int damage);

void enemy_kill(enemy_t *e);

#endif
