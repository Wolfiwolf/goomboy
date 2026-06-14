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
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "gayinvaders.h"
#include "inputs.h"

/* Put game window in center of screen */
#define SCREEN_START_X ((LCD_W_SIZE / 2) - (SCREEN_W / 2))
#define SCREEN_START_Y ((LCD_H_SIZE / 2) - (SCREEN_H / 2))

static uint16_t *_screen_buff;

static uint16_t _prev_btn_map;

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
	size_t t = pdTICKS_TO_MS(xTaskGetTickCount());

	// Shooting
	/*
	if (t - _prev_shoot_t > 550 && t - _prev_shoot_t < 650 ) {
		if (input == INPUT_FIRE_NORMAL)
			return INPUT_STATE_ON;
	}

	if (t - _prev_shoot_t > 1000)
		_prev_shoot_t = t;
		*/


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
}

void *gayinvaders_malloc(size_t sz)
{
	multi_heap_info_t meminfo;

	heap_caps_get_info(&meminfo, MALLOC_CAP_8BIT);

	// ESP_LOGE("", "ALLOCATION: Free mem: %lu, big block: %lu, allocating: %lu", meminfo.total_free_bytes, meminfo.largest_free_block, sz);
	void *ptr = heap_caps_malloc(sz, MALLOC_CAP_8BIT);

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

	lcd_clear(32, 32);
	lcd_clear(16, 16);

	ret = gpio_init();
	if (ret) {
		ESP_LOGE("", "GPIO init failed!");
		stall();
	}

	ret = sdcard_init();
	if (ret) {
		ESP_LOGE("", "SD card mount failed! Is DOOM1.WAD on a FAT-formatted card?");
		stall();
	}

	srand(xTaskGetTickCount());

	_prev_t = pdTICKS_TO_MS(xTaskGetTickCount());
	_prev_shoot_t = pdTICKS_TO_MS(xTaskGetTickCount());

	gayinvaders_main(argc, argv);
}
