#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "gameobject.h"

void physics_init(void);

void physics_register(game_object_t *pobj);

void physics_update(float dt);

#endif
