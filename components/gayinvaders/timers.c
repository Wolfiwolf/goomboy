#include <stdio.h>
#include "timers.h"
#include "gayinvaders.h"
#include "llist.h"

typedef struct {
	int duration;
	int counter;
	bool loop;
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

		tim->counter -= dt * 1000; 

		if (tim->counter > 0) {
			current = current->next;
			continue;
		}

		tim->on_finish(tim->data);
		if (tim->loop) {
			tim->counter = tim->duration;
			continue;
		}

		tmp = current;
		current = current->next;

		llist_remove_node(&_timers, tmp);

		gayinvaders_free(tim);
	}
}

timer_handle_t *timers_start(int duration, bool loop,
			     void *data, void (*on_finish)(void *data))
{
	timer_obj_t *tim = gayinvaders_malloc(sizeof(timer_obj_t));

	tim->duration = duration;
	tim->counter = duration;
	tim->loop = loop;
	tim->data = data;
	tim->on_finish = on_finish;

	llist_push_back(&_timers, tim);

	return _timers.tail;
}

void timers_stop(timer_handle_t *tim_handle)
{
	llist_node_t *node = tim_handle;
	timer_obj_t *tim = node->obj;

	llist_remove_node(&_timers, node);

	gayinvaders_free(tim);
}
