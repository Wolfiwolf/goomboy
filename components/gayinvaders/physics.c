#include <stdio.h>
#include <stdlib.h>
#include "physics.h"
#include "gayinvaders.h"
#include "llist.h"

static llist_t _physics_objects;
static size_t _prev_t;

void physics_init(void)
{
	llist_init(&_physics_objects);
}

void physics_register(game_object_t *go)
{
	llist_push_back(&_physics_objects, go);
}

void physics_update(void)
{
	llist_node_t *cur_node;
	float dt;

	dt = ((float)gayinvaders_get_ms() - (float)_prev_t) / 1000.0f;
	_prev_t = gayinvaders_get_ms();

	llist_for(_physics_objects, cur_node) {
		game_object_t *po = cur_node->obj;

		if (!po->active)
			continue;

		po->vx -= dt * po->resistance * po->vx;
		po->vy -= dt * po->resistance * po->vy;

		po->vx += dt * po->ax;
		po->vy += dt * po->ay;

		po->x += dt * po->vx;
		po->y += dt * po->vy;

		/*
		printf("pos: (%.3f, %.3f)\n", po->x, po->y);
		printf("vel: (%.3f, %.3f)\n", po->vx, po->vy);
		printf("acc: (%.3f, %.3f)\n", po->ax, po->ay);
		*/
	}
}
