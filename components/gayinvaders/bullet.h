#ifndef BULLET_H_
#define BULLET_H_

#include "gameobject.h"
#include "renderer.h"

typedef struct {
	game_object_t go;
	render_obj_t ro;

	int damage;
} bullet_t;

void bullet_init();

#endif
