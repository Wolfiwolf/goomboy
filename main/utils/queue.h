#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t *buf;
	int      item_size;
	int      capacity;
	int      head;
	int      tail;
	int      count;
} queue_t;

void queue_init(queue_t *q, void *buf, size_t item_size, size_t capacity);

int queue_push(queue_t *q, const void *item);

int queue_pop(queue_t *q, void *item);

int queue_peek(const queue_t *q, void *item);

int queue_is_empty(const queue_t *q);

int queue_is_full(const queue_t *q);

size_t queue_count(const queue_t *q);

#endif
