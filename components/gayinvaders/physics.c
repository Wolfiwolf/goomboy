#include <stdio.h>
#include <stdlib.h>
#include "physics.h"
#include "gayinvaders.h"
#include "llist.h"

void physics_init(void)
{
	// Empty
}

void physics_update(game_object_t *go, float dt)
{
	go->vx -= dt * go->resistance * go->vx;
	go->vy -= dt * go->resistance * go->vy;

	go->vx += dt * go->ax;
	go->vy += dt * go->ay;

	go->x += dt * go->vx;
	go->y += dt * go->vy;
}
