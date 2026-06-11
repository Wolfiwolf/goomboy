#ifndef LLIST_H_
#define LLIST_H_

#define llist_for(list, lnode) for(lnode = list.head; lnode; lnode = lnode->next)

struct llist_node;

typedef struct llist_node llist_node_t;

struct llist_node {
	llist_node_t *next;
	llist_node_t *prev;
	void *obj;
};

typedef struct {
	llist_node_t *head;
	llist_node_t *tail;
} llist_t;

void llist_init(llist_t *l);

void llist_push_back(llist_t *l, void *obj);

void llist_remove_node(llist_t *l, llist_node_t *node);

#endif
