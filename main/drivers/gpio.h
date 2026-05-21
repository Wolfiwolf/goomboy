#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

#define GPIO_BTN_FIRE   0
#define GPIO_BTN_ACTION 1
#define GPIO_BTN_UP     2
#define GPIO_BTN_DOWN   3
#define GPIO_BTN_LEFT   4
#define GPIO_BTN_RIGHT  5

int gpio_init(void);

int gpio_read(uint16_t *bitmap);

#endif
