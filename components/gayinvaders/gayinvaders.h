#ifndef GAYINVADERS_H_
#define GAYINVADERS_H_

#include <stdlib.h>
#include <stdint.h>
#include "inputs.h"

/* CONFIG */
#define SCREEN_W 340
#define SCREEN_H 320

#define SCREEN_W_HALF (SCREEN_W/2)
#define SCREEN_H_HALF (SCREEN_H/2)

#define SCREEN_FRAMES_X 4
#define SCREEN_FRAMES_Y 4
#define SCREEN_FRAMES_CNT (FRAMES_X + FRAMES_Y)


void gayinvaders_main(int argc, char *argv[]);

/* Call this when buffer has been displayed.
 * This lets the rendering engine know that
 * the screen buffers can be changed */
void gayinvaders_render_finished(void);

/* Implement */

void gayinvaders_render(const uint16_t ***screen_buff);

size_t gayinvaders_get_ms(void);

void gayinvaders_sleep_ms(size_t ms);

input_state_t gayinvaders_get_input(input_t input);

void *gayinvaders_malloc(size_t sz);

void gayinvaders_free(void *ptr);

size_t gayinvaders_free_mem(void);

#endif
