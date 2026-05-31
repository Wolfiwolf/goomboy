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
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "doomgeneric.h"

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


void DG_Init()
{

}

void DG_DrawFrame()
{
	lcd_draw(SCREEN_START_X, SCREEN_START_Y, DOOMGENERIC_RESX, DOOMGENERIC_RESY, DG_ScreenBuffer);
}

void DG_SleepMs(uint32_t ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t DG_GetTicksMs()
{
	return xTaskGetTickCount();
}

int DG_GetKey(int* pressed, unsigned char* key)
{
	return 0;
}

void DG_SetWindowTitle(const char * title)
{
	// Empty
}

void app_main(void)
{
	const char *argv[3] = {
		"doomgeneric",
		"-iwad",
		"/sdcard/10M.wad"
	};
	int argc = 3;
	int ret;

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

	ret = sdcard_init();
	if (ret) {
		ESP_LOGE("", "SD card mount failed! Is DOOM1.WAD on a FAT-formatted card?");
		stall();
	}

	doomgeneric_Create(argc, argv);

	while (1) {
		doomgeneric_Tick();
	}
}
/* Biggest .bss
 visplanes    │ 84,992 B │ RAM (.bss)             │ r_plane.c │
├──────────────┼──────────┼────────────────────────┼───────────┤
│ finesine     │ 40,960 B │ flash (.rodata, const) │ tables.c  │
├──────────────┼──────────┼────────────────────────┼───────────┤
│ openings     │ 40,960 B │ RAM (.bss)             │ r_plane.c │
├──────────────┼──────────┼────────────────────────┼───────────┤
│ states       │ 27,076 B │ RAM (.data)            │ info.c    │
├──────────────┼──────────┼────────────────────────┼───────────┤
│ ticdata      │ 20,480 B │ RAM (.bss)             │ d_loop.c  │
├──────────────┼──────────┼────────────────────────┼───────────┤
│ finetangent  │ 16,384 B │ flash (const)          │ tables.c  │
├──────────────┼──────────┼────────────────────────┼───────────┤
│ viewangletox │ 16,384 B │ RAM (.bss)             │ r_main.c
 * */
