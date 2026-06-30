#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

#define GPIO_BTN_UP     0
#define GPIO_BTN_LEFT   1
#define GPIO_BTN_RIGHT  2
#define GPIO_BTN_DOWN   3
#define GPIO_BTN_SELECT 4
#define GPIO_BTN_START  5
#define GPIO_BTN_B      6
#define GPIO_BTN_Y      7
#define GPIO_BTN_A      8
#define GPIO_BTN_X      9

int gpio_init(void);

/* Read all 16 pins. Bit n = pin n; for the button pins a press reads as 1. */
int gpio_read(uint16_t *bitmap);

/* Set pin directions: bit n = 1 -> input, 0 -> output.
 * (Named gpio_set_dir to avoid clashing with ESP-IDF's gpio_set_direction.) */
int gpio_set_dir(uint16_t input_mask);

/* Drive the output latch (only affects pins configured as outputs). */
int gpio_write(uint16_t bitmap);

#endif
