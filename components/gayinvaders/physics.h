#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "gameobject.h"

void physics_init(void);

void physics_update(game_object_t *go, float dt);

#endif
