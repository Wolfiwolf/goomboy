#ifndef POWERUP_H_
#define POWERUP_H_

#include "gameobject.h"
#include "renderer.h"

typedef enum {
	POWERUP_TYPE_HEALTH = 0,
	POWERUP_TYPE_BOMB,
	POWERUP_TYPE_CNT,
} powerup_type_t;

typedef struct {
	game_object_t go;
	render_obj_t ro;

	powerup_type_t type;
	int speed;

	int collision_radius;
} powerup_t;

void powerup_init(powerup_t *b);

void powerup_destroy(powerup_t *b);

void powerup_activate(powerup_t *b, powerup_type_t type,
		      int x, int y);

void powerup_diactivate(powerup_t *b);

void powerup_update(powerup_t *b, float dt);

#endif
