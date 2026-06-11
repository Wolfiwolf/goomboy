#include <stdio.h>
#include "timers.h"
#include "gayinvaders.h"
#include "llist.h"

typedef struct {
	int duration;
	void *data;
	void (*on_finish)(void *data);
} timer_obj_t;

static llist_t _timers;

void timers_init(void)
{
	llist_init(&_timers);
}

void timers_update(float dt)
{
	llist_node_t *current;

	current = _timers.head;
	while (current) {
		timer_obj_t *tim = current->obj;
		llist_node_t *tmp;

		tim->duration -= dt * 1000; 

		if (tim->duration > 0) {
			current = current->next;
			continue;
		}

		tim->on_finish(tim->data);

		tmp = current;
		current = current->next;

		llist_remove_node(&_timers, tmp);

		gayinvaders_free(tim);
	}
}

void timers_start(int duration, void *data,
		  void (*on_finish)(void *data))
{
	timer_obj_t *tim = gayinvaders_malloc(sizeof(timer_obj_t));

	tim->duration = duration;
	tim->data = data;
	tim->on_finish = on_finish;

	llist_push_back(&_timers, tim);
}
