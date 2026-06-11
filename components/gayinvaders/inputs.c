#include "inputs.h"
#include "gayinvaders.h"

#define DEBOUNCE_DELAY 10

static inputs_on_handler_t _input_on_handlers[INPUT_COUNT] = {};

static size_t _input_times[INPUT_COUNT];
static input_state_t _input_states[INPUT_COUNT];

void inputs_set_on_handler(input_t input, inputs_on_handler_t handler)
{
	_input_on_handlers[input] = handler;
}

void inputs_update(void)
{
	int input;

	for (input = 0; input < INPUT_COUNT; ++input) {
		input_state_t state = gayinvaders_get_input(input);
		size_t t = gayinvaders_get_ms();

		if (state == _input_states[input])
			continue;
		
		if (t - _input_states[input] < DEBOUNCE_DELAY)
			continue;

		_input_states[input] = state;
		_input_times[input] = t;

		if (_input_on_handlers[input] && state == INPUT_STATE_ON)
			_input_on_handlers[input]();
	}
}

input_state_t inputs_get(input_t input)
{
	return _input_states[input];
}
