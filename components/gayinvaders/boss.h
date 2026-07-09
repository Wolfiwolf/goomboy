#ifndef BOSS_H_
#define BOSS_H_

#include "gameobject.h"
#include "renderer.h"
#include "bullet.h"
#include "player.h"

#define BOSS_HEALTH_BLOCKS_CNT 20

typedef enum {
	BOSS_PHAZE_IDLE = 0,
	BOSS_PHAZE_0,
	BOSS_PHAZE_1,
	BOSS_PHAZE_CNT,
} boss_phaze_t;

typedef struct {
	game_object_t go;
	render_obj_t ro;
} boss_health_block_t;

typedef struct {
	game_object_t go;
	render_obj_t image;
	boss_health_block_t health_blocks[BOSS_HEALTH_BLOCKS_CNT];

	bool dead;

	int health;
	int collision_radius;

	boss_phaze_t phaze;
	boss_phaze_t prev_phaze;
	size_t prev_phaze_switch_t;

	size_t prev_shoot_t;

	size_t prev_teleport_t;
} boss_t;

void boss_init(boss_t *b);

void boss_update(boss_t *b, float dt, bullet_t *bullets, int bullets_cnt,
		 player_t *player);

void boss_render(boss_t *b);

void boss_destroy(boss_t *b);

void boss_damage(boss_t *b, int amount);

#endif
