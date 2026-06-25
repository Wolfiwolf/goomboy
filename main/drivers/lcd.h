#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

#define LCD_W_SIZE 480
#define LCD_H_SIZE 320

int lcd_init(void (*buffer_transfered_callback)(void));

// pixels: RGB565 buffer, width * height * 2 bytes
int lcd_draw(int x, int y, int w, int h, const uint16_t *pixels);

void lcd_clear(int sizex, int sizey, uint16_t color);

#endif
