#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <stdbool.h>

typedef struct {
	float x, y;
	float vx, vy;
	float ax, ay;

	float resistance;

	bool active;
} game_object_t;

#endif
