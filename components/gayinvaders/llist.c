#include <stdlib.h>
#include "llist.h"
#include "gayinvaders.h"

void llist_init(llist_t *l)
{
	l->head = NULL;
	l->tail = NULL;
}

void llist_push_back(llist_t *l, void *obj)
{
	llist_node_t *new_node = gayinvaders_malloc(sizeof(llist_node_t));
	new_node->obj = obj;

	if (!l->head) {
		l->head = new_node;
		l->tail = new_node;
		new_node->next = NULL;
		new_node->prev = NULL;
		return;
	}

	l->tail->next = new_node;
	new_node->prev = l->tail;
	new_node->next = NULL;
	l->tail = new_node;
}

void llist_remove_node(llist_t *l, llist_node_t *node)
{
	if (node->prev)
		node->prev->next = node->next;

	if (node->next)
		node->next->prev = node->prev;

	if (node == l->head)
		l->head = node->next;

	if (node == l->tail)
		l->tail = node->prev;

	gayinvaders_free(node);
}
