#ifndef INPUTS_H_
#define INPUTS_H_

typedef enum {
	INPUT_STATE_ON,
	INPUT_STATE_OFF,
} input_state_t;

typedef enum {
	INPUT_UP,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_DOWN,
	INPUT_SELECT,
	INPUT_START,
	INPUT_FIRE_NORMAL,
	INPUT_FIRE_BOMB,
	INPUT_X,
	INPUT_COUNT,
} input_t;

typedef void(*inputs_on_handler_t)(void);

void inputs_set_on_handler(input_t input, inputs_on_handler_t handler);

void inputs_update(void);

input_state_t inputs_get(input_t input);

#endif
