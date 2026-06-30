/**
 * The documentation of the board:
 * https://www.lcdwiki.com/4.0inch_ESP32-32E_Display
 */

#include "drivers/gpio.h"
#include "drivers/lcd.h"
#include "drivers/sdcard.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "system.h"
#include "utils/queue.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "gayinvaders.h"
#include "inputs.h"

/* Put game window in center of screen */
#define SCREEN_START_X ((LCD_W_SIZE / 2) - (SCREEN_W / 2))
#define SCREEN_START_Y ((LCD_H_SIZE / 2) - (SCREEN_H / 2))

static bool first = true;
static int _buffers_transfering = 0;

static void stall(void)
{
	while (1)
		vTaskDelay(pdMS_TO_TICKS(100));
}


static void _lcd_buffer_transfered_handler(void)
{
	_buffers_transfering -= 1;

	if (_buffers_transfering == 0)
		gayinvaders_render_finished();
}

void gayinvaders_render(const uint16_t ***screen_buff)
{
	int x,y;

	if (first) {
		_buffers_transfering = 0;
		first = false;
	}

	while (_buffers_transfering) { }


	for (x = 0; x < SCREEN_FRAMES_X; ++x) {
		for (y = 0; y < SCREEN_FRAMES_Y; ++y) {
			_buffers_transfering += 1;

			lcd_draw(
				SCREEN_START_X+(x*(SCREEN_W/SCREEN_FRAMES_X)),
				SCREEN_START_Y+(y*(SCREEN_H/SCREEN_FRAMES_Y)),
				 SCREEN_W/SCREEN_FRAMES_X,
				 SCREEN_H/SCREEN_FRAMES_Y, screen_buff[x][y]);
		}
	}
}

void gayinvaders_render_direct(const uint16_t *screen_buff,
			       int xoff, int yoff,
			       int w, int h)
{
	lcd_draw(xoff, yoff, w, h, screen_buff);
}

size_t gayinvaders_get_ms(void)
{
	return pdTICKS_TO_MS(xTaskGetTickCount());
}

void gayinvaders_sleep_ms(size_t ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}

static bool dir = true;
static size_t _prev_t;
static size_t _prev_shoot_t;

input_state_t gayinvaders_get_input(input_t input)
{
	/*
	*/
	uint16_t btn_bitmap;
	bool is_on = false;
	int btn_shift;
	int ret;

	ret = gpio_read(&btn_bitmap);
	if (ret) {
		ESP_LOGE("", "GPIO read fail");
		return INPUT_STATE_OFF;
	}

	switch (input) {
	case INPUT_LEFT:
		btn_shift = GPIO_BTN_LEFT;
		break;
	case INPUT_RIGHT:
		btn_shift = GPIO_BTN_RIGHT;
		break;
	case INPUT_DOWN:
		btn_shift = GPIO_BTN_DOWN;
		break;
	case INPUT_UP:
		btn_shift = GPIO_BTN_UP;
		break;
	case INPUT_FIRE_NORMAL:
		btn_shift = GPIO_BTN_Y;
		break;
	case INPUT_FIRE_BOMB:
		btn_shift = GPIO_BTN_B;
		break;
	case INPUT_SHIELD:
		btn_shift = GPIO_BTN_X;
		break;
	case INPUT_RAPIDFIRE:
		btn_shift = GPIO_BTN_A;
		break;
	case INPUT_START:
		btn_shift = GPIO_BTN_START;
		break;
	case INPUT_SELECT:
		btn_shift = GPIO_BTN_SELECT;
		break;
	default:
		return INPUT_STATE_OFF;
	}

	is_on = !(btn_bitmap & (1<<btn_shift));

	return is_on ? INPUT_STATE_ON : INPUT_STATE_OFF;

	/* Simulation
	size_t t = pdTICKS_TO_MS(xTaskGetTickCount());
	// Shooting
	if (t - _prev_shoot_t > 550 && t - _prev_shoot_t < 650 ) {
		if (input == INPUT_FIRE_NORMAL)
			return INPUT_STATE_ON;
	}

	if (t - _prev_shoot_t > 1000)
		_prev_shoot_t = t;


	// Moving
	if (t - _prev_t > 3000) {
		dir = !dir;
		_prev_t = t;
	}

	if (dir && input == INPUT_LEFT)
		return INPUT_STATE_ON;
	else if (!dir && input == INPUT_RIGHT)
		return INPUT_STATE_ON;

	return INPUT_STATE_OFF;
 */
}

void *gayinvaders_malloc(size_t sz)
{
	void *ptr = heap_caps_malloc(sz, MALLOC_CAP_DEFAULT);

	if (!ptr)
		ESP_LOGE("", "MALLOC OF %lu FAILED!", sz);

	return ptr;
}

void gayinvaders_free(void *ptr)
{
	heap_caps_free(ptr);
}

size_t gayinvaders_free_mem(void)
{
	return sys_get_free_mem();
}

#define SIDEBAR_SIZEX (LCD_W_SIZE - SCREEN_W)/2

static void _draw_side_bard(void)
{
	// Pride flag colors (BGR565 — panel is BGR), spread across the 70px sidebar
	uint16_t colors[SIDEBAR_SIZEX] = {
		0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, 0x001C, // red    #E40303
		0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, 0x047F, // orange #FF8C00
		0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, 0x077F, // yellow #FFED00
		0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, 0x2400, // green  #008026
		0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, 0xFA60, // blue   #004DFF
		0x802E, 0x802E, 0x802E, 0x802E, 0x802E, 0x802E, 0x802E, 0x802E, 0x802E, 0x802E, 0x802E, // violet #750787
	};
	const int szy =LCD_H_SIZE/2;
	uint16_t buff[szy];
	uint16_t rbuff[szy];
	int x;

	for (x = 0; x < SIDEBAR_SIZEX; ++x) {
		uint16_t color = colors[x];
		uint16_t rcolor = colors[SIDEBAR_SIZEX - 1 - x]; // reversed for right side
		int i;

		color = (color >> 8) | ((color & 0xFF) << 8);
		rcolor = (rcolor >> 8) | ((rcolor & 0xFF) << 8);

		for (i = 0; i < szy; ++i) {
			buff[i] = color;
			rbuff[i] = rcolor;
		}

		// left sidebar
		lcd_draw(x, 0, 1, szy, buff);
		lcd_draw(x, LCD_H_SIZE/2, 1, szy, buff);

		// right sidebar (reversed)
		lcd_draw(SCREEN_START_X + SCREEN_W + x, 0, 1, szy, rbuff);
		lcd_draw(SCREEN_START_X + SCREEN_W + x, LCD_H_SIZE/2, 1, szy, rbuff);
		vTaskDelay(pdMS_TO_TICKS(10));
	}

}

void app_main(void)
{
	const char *argv[3] = {
		"gayinvaders",
		"/sdcard/game.wd",
	};
	int argc = 2;
	int ret;


	ret = lcd_init(_lcd_buffer_transfered_handler);
	if (ret) {
		ESP_LOGE("", "LCD init failed!");
		stall();
	}

	lcd_clear(32, 32, 0xFFFF);

	ret = gpio_init();
	if (ret) {
		uint16_t color = 0b11111; // RED
		lcd_clear(16, 16, (color >> 8) | ((color & 0xFF) << 8));
		stall();
	}

	ret = sdcard_init();
	if (ret) {
		ESP_LOGE("", "SD card mount failed! Is DOOM1.WAD on a FAT-formatted card?");
		stall();
	}

	_draw_side_bard();

	srand(xTaskGetTickCount());

	_prev_t = pdTICKS_TO_MS(xTaskGetTickCount());
	_prev_shoot_t = pdTICKS_TO_MS(xTaskGetTickCount());

	gayinvaders_main(argc, argv);
}
