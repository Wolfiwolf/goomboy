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

void gayinvaders_main(int argc, char *argv[]);

/* Submit a content-space RGB565 rectangle and wait until the LCD has consumed it. */
int gayinvaders_present(const uint16_t *pixels, int x, int y, int w, int h);

size_t gayinvaders_get_ms(void);

void gayinvaders_sleep_ms(size_t ms);

input_state_t gayinvaders_get_input(input_t input);

void *gayinvaders_malloc(size_t sz);

void gayinvaders_free(void *ptr);

size_t gayinvaders_free_mem(void);

#endif
