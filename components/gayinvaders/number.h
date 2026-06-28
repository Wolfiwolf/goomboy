#ifndef NUMBER_H_
#define NUMBER_H_

#include "gameobject.h"
#include "renderer.h"

#define NUMBER_MAX_DIGITS 3

typedef struct {
	game_object_t go;
	render_obj_t ro;

	int val;
} number_digit_t;

typedef struct {
	game_object_t go;

	number_digit_t digits[NUMBER_MAX_DIGITS];

	int val;
	int num_of_digits;
} number_t;

void number_init(number_t *n, int x, int y);

void number_set_val(number_t *n, int val);

void number_destroy(number_t *n);

void number_render(number_t *n);

#endif
