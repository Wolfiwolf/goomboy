#include <string.h>
#include "queue.h"

void queue_init(queue_t *q, void *buf, size_t item_size, size_t capacity)
{
	q->buf = buf;
	q->item_size = item_size;
	q->capacity = capacity;
	q->head = 0;
	q->tail = 0;
	q->count = 0;
}

int queue_push(queue_t *q, const void *item)
{
	if (queue_is_full(q))
		return -1;

	memcpy(q->buf + q->tail * q->item_size, item, q->item_size);
	q->tail = (q->tail + 1) % q->capacity;
	q->count++;

	return 0;
}

int queue_pop(queue_t *q, void *item)
{
	if (queue_is_empty(q))
		return -1;

	memcpy(item, q->buf + q->head * q->item_size, q->item_size);
	q->head = (q->head + 1) % q->capacity;
	q->count--;

	return 0;
}

int queue_peek(const queue_t *q, void *item)
{
	if (queue_is_empty(q))
		return -1;

	memcpy(item, q->buf + q->head * q->item_size, q->item_size);

	return 0;
}

int queue_is_empty(const queue_t *q)
{
	return q->count == 0;
}

int queue_is_full(const queue_t *q)
{
	return q->count == q->capacity;
}

size_t queue_count(const queue_t *q)
{
	return q->count;
}
