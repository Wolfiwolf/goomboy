#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <stdbool.h>

typedef enum {
	GAME_OBJECT_TYPE_PLAYER,
	GAME_OBJECT_TYPE_ENEMY,
	GAME_OBJECT_TYPE_BULLET,
	GAME_OBJECT_TYPE_POWERUP,
	GAME_OBJECT_TYPE_CNT,
} game_object_type_t;

typedef struct {
	game_object_type_t type;

	float x, y;
	float vx, vy;
	float ax, ay;

	float resistance;

	bool active;
} game_object_t;

#endif
