#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

#define LCD_W_SIZE 320
#define LCD_H_SIZE 480

int lcd_init(void);

// pixels: RGB565 buffer, width * height * 2 bytes
int lcd_draw(int x, int y, int w, int h, const uint16_t *pixels);

#endif
