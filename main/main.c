/**
 * The documentation of the board:
 * https://www.lcdwiki.com/4.0inch_ESP32-32E_Display
 */

#include "doom/doomgeneric.h"
#include "doom/doomkeys.h"
#include "drivers/gpio.h"
#include "drivers/lcd.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "utils/queue.h"
#include <inttypes.h>

/* Put game window in center of screen */
#define SCREEN_START_X ((LCD_W_SIZE / 2) - (DOOMGENERIC_RESX / 2))
#define SCREEN_START_Y ((LCD_H_SIZE / 2) - (DOOMGENERIC_RESY / 2))

#define BTN_QUEUE_MAX 16

typedef struct {
	int pressed;
	unsigned char key;
} btn_queue_element_t;

static queue_t _btn_queue;
static uint8_t _btn_queue_buffer[sizeof(btn_queue_element_t) * BTN_QUEUE_MAX];

static uint16_t _prev_btn_map;

void stall(void)
{
	while (1)
		esp_task_wdt_reset();
}

void DG_Init(void)
{
	queue_init(&_btn_queue, _btn_queue_buffer,
		   sizeof(btn_queue_element_t),
		   BTN_QUEUE_MAX);

	_prev_btn_map = 0;
}

static int _btn_to_doomkey(int key)
{
	switch (key) {
	case GPIO_BTN_FIRE:
		return KEY_FIRE;
	case GPIO_BTN_ACTION:
		return KEY_USE;
	default:
		return 0;
	}
}

static void _process_keys(void)
{
	btn_queue_element_t btn_qe;
	uint16_t diff_map;
	uint16_t btn_map;
	int ret;
	int i;

	ret = gpio_read(&btn_map);
	if (ret) {
		ESP_LOGE("", "Gpio read fail: %d", ret);
		return;
	}

	diff_map = _prev_btn_map ^ btn_map;

	if (diff_map == 0)
		return;

	for (i = 0; i < 16; ++i) {
		if (!(diff_map & (1 << i)))
			continue;

		btn_qe.key = _btn_to_doomkey(i);
		btn_qe.pressed = btn_map & (1 << i);
		queue_push(&_btn_queue, &btn_qe);
	}
	
	_prev_btn_map = btn_map;
}

void DG_DrawFrame(void)
{
	int ret;

	ret = lcd_draw(SCREEN_START_X, SCREEN_START_Y,
		       DOOMGENERIC_RESX, DOOMGENERIC_RESY,
		       (uint16_t*)DG_ScreenBuffer);

	if (ret)
		ESP_LOGE("", "LCD draw has failed %d", ret);

	_process_keys();
}

void DG_SleepMs(uint32_t ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t DG_GetTicksMs(void)
{
	return pdTICKS_TO_MS(xTaskGetTickCount());
}

int DG_GetKey(int *pressed, unsigned char *key)
{
	btn_queue_element_t btn_qe;

	if (queue_pop(&_btn_queue, &btn_qe))
		return 0;

	*pressed = btn_qe.pressed;
	*key = btn_qe.key;

	return 1;
}

void DG_SetWindowTitle(const char *title)
{
	/* Empty */
}

void app_main(void)
{
	char *argv[] = {
		"doomgeneric",
		"-iwad"
		"DOOM.WAD"
	};
	int argc = 2;
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

	doomgeneric_Create(argc, argv);

	for (;;)
		doomgeneric_Tick();
}
