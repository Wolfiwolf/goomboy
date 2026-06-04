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
#include "doomgeneric.h"
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


void DG_Init()
{}
void DG_DrawFrame()
{}
void DG_SleepMs(uint32_t ms)
{}
uint32_t DG_GetTicksMs()
{
	return 0;
}
int DG_GetKey(int* pressed, unsigned char* key)
{
	return 0;
}
void DG_SetWindowTitle(const char * title)
{}

void app_main(void)
{
	const char *argv[3] = {
		"doomgeneric",
		"-iwad",
		"/sdcard/BLASPHEM.WAD"
	};
	int argc = 3;
	int ret;

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

/*
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ visplanes      │   83 KB │ r_plane.c — visplane_t[MAXVISPLANES] (128 planes × ~664 B) │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ openings       │   40 KB │ r_plane.c — MAXOPENINGS short array for visplane clipping  │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ ticdata        │   20 KB │ d_loop.c — netgame tic command ring buffer (BACKUPTICS)    │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ viewangletox   │   16 KB │ r_main.c — int[FINEANGLES/2] (2048 ints)                   │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ drawsegs       │   12 KB │ r_bsp.c — drawseg_t[MAXDRAWSEGS]                           │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ zlight         │    8 KB │ r_main.c — light-scale lookup [LIGHTLEVELS][MAXLIGHTZ]     │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ vissprites     │  7.5 KB │ r_things.c — vissprite_t[MAXVISSPRITES]                    │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ captured_stats │ 6.25 KB │ profiling/stats buffer                                     │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ columnofs      │  4.4 KB │ r_draw.c — column offset table                             │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ ylookup        │  3.3 KB │ r_draw.c — per-row framebuffer pointers                    │
├────────────────┼─────────┼────────────────────────────────────────────────────────────┤
│ scalelight     │    3 KB │ r_main.c — light-scale pointers                            │
└────────────────┴─────────┴────────────────────────────────────────────────────────────┘
*/
