/**
 * The documentation of the board:
 * https://www.lcdwiki.com/4.0inch_ESP32-32E_Display
 */

#include "drivers/gpio.h"
#include "drivers/lcd.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_task_wdt.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "system.h"
#include "utils/queue.h"
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define SQUARE_COLOR 0xF800  /* red, RGB565 */

/* Put game window in center of screen */
#define SCREEN_START_X ((LCD_H_SIZE / 2) - (DOOMGENERIC_RESX / 2))
#define SCREEN_START_Y ((LCD_W_SIZE / 2) - (DOOMGENERIC_RESY / 2))

#define BTN_QUEUE_MAX 16

typedef struct {
	int pressed;
	unsigned char key;
} btn_queue_element_t;

static queue_t _btn_queue;
static uint8_t _btn_queue_buffer[sizeof(btn_queue_element_t) * BTN_QUEUE_MAX];

// static uint16_t _screen_buff[LCD_W_SIZE * LCD_H_SIZE];
static uint16_t *_screen_buff;

static uint16_t _prev_btn_map;

void stall(void)
{
	while (1)
		vTaskDelay(pdMS_TO_TICKS(100));
}


static int posx = 0;
static int dir = 2;

void app_main(void)
{
	char *argv[] = {
		"doomgeneric",
		"-iwad"
		"DOOM.WAD"
	};
	int argc = 2;
	int ret;
	esp_pm_config_t pm = {
		.max_freq_mhz = 240,
		.min_freq_mhz = 240,
		.light_sleep_enable = false,
	};
	esp_pm_configure(&pm);

	ESP_LOGE("", "Free MEM: %lu", sys_get_free_mem());

	ret = lcd_init();
	if (ret) {
		ESP_LOGE("", "LCD init failed!");
		stall();
	}

	ret = gpio_init();
	if (ret) {
		ESP_LOGE("", "GPIO init failed!");
		stall();
	}

	while (1) {
		vTaskDelay(pdMS_TO_TICKS(40));
	}
}
