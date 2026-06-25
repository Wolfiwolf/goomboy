#include "lcd.h"

#include "esp_heap_caps.h"
#include "esp_lcd_types.h"
#include "freertos/FreeRTOS.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7796.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

#define TAG "LCD"

#define LCD_HOST        SPI2_HOST
#define LCD_SCLK        14
#define LCD_MOSI        13
#define LCD_MISO        12
#define LCD_DC          2
#define LCD_CS          15
#define LCD_RST         12
#define LCD_BL          27
#define LCD_BL_ON       1

#define LCD_CLK_HZ      (80 * 1000 * 1000)
#define LCD_CMD_BITS    8
#define LCD_PARAM_BITS  8

static esp_lcd_panel_handle_t s_panel;

void (*_buffer_transfered_callback)(void);

static bool lcd_on_trans_done(esp_lcd_panel_io_handle_t io,
			      esp_lcd_panel_io_event_data_t *edata,
			      void *user_ctx)
{
	if (_buffer_transfered_callback)
		_buffer_transfered_callback();
	return true;
}

int lcd_init(void (*buffer_transfered_callback)(void))
{
	gpio_config_t bl_cfg = {
	    .mode = GPIO_MODE_OUTPUT,
	    .pin_bit_mask = 1ULL << LCD_BL,
	};
	ESP_ERROR_CHECK(gpio_config(&bl_cfg));
	gpio_set_level(LCD_BL, !LCD_BL_ON);

	spi_bus_config_t buscfg = {
	    .sclk_io_num = LCD_SCLK,
	    .mosi_io_num = LCD_MOSI,
	    .miso_io_num = LCD_MISO,
	    .quadwp_io_num = -1,
	    .quadhd_io_num = -1,
	    .max_transfer_sz = 320 * 200 * sizeof(uint16_t),
	};
	ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

	esp_lcd_panel_io_handle_t io;
	esp_lcd_panel_io_spi_config_t io_cfg = {
	    .dc_gpio_num = LCD_DC,
	    .cs_gpio_num = LCD_CS,
	    .pclk_hz = LCD_CLK_HZ,
	    .lcd_cmd_bits = LCD_CMD_BITS,
	    .lcd_param_bits = LCD_PARAM_BITS,
	    .spi_mode = 0,
	    .trans_queue_depth = 1,
	    .on_color_trans_done = lcd_on_trans_done,
	};

	ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_cfg, &io));

	esp_lcd_panel_dev_config_t panel_cfg = {
	    .reset_gpio_num = LCD_RST,
	    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
	    .bits_per_pixel = 16,
	};

	ESP_LOGI(TAG, "Install ST7796 panel driver");
	ESP_ERROR_CHECK(esp_lcd_new_panel_st7796(io, &panel_cfg, &s_panel));
	ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
	ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));
	ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel, true));

	esp_lcd_panel_swap_xy(s_panel, true);

	gpio_set_level(LCD_BL, LCD_BL_ON);

	_buffer_transfered_callback = buffer_transfered_callback;

	return 0;
}

int lcd_draw(int x, int y, int w, int h, const uint16_t *pixels)
{
	esp_err_t err = esp_lcd_panel_draw_bitmap(s_panel, x, y, x + w, y + h, pixels);
	if (err != ESP_OK)
		return err;
	return ESP_OK;
}

void lcd_clear(int sizex, int sizey, uint16_t color)
{
	uint16_t *buffer = heap_caps_malloc(sizex*sizey*2, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
	int x, y;
	int i;


	for (i = 0; i < sizex*sizey; ++i)
		buffer[i] = color;

	for (x = 0; x < LCD_W_SIZE; x +=sizex) {
		for (y = 0; y < LCD_H_SIZE; y +=sizey) {
			lcd_draw(x, y, sizex, sizey, buffer);
		}
	}

	heap_caps_free(buffer);
}
