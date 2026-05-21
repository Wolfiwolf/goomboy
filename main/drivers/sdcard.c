#include "sdcard.h"

#include <stdio.h>
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define TAG "SDCARD"
#define MOUNT_POINT "/sdcard"
#define SD_HOST SPI3_HOST

#define SPI_CS 5
#define SPI_MOSI 23
#define SPI_CLK 18
#define SPI_MISO 19

static sdmmc_card_t *s_card;

int sdcard_init(void)
{
	spi_bus_config_t buscfg = {
	    .mosi_io_num = SPI_MOSI,
	    .miso_io_num = SPI_MISO,
	    .sclk_io_num = SPI_CLK,
	    .quadwp_io_num = -1,
	    .quadhd_io_num = -1,
	};
	ESP_ERROR_CHECK(spi_bus_initialize(SD_HOST, &buscfg, SPI_DMA_CH_AUTO));

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	host.slot = SD_HOST;

	sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
	slot.gpio_cs = SPI_CS;
	slot.host_id = SD_HOST;

	esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
	    .format_if_mount_failed = false,
	    .max_files = 5,
	    .allocation_unit_size = 16 * 1024,
	};

	esp_err_t ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot, &mount_cfg, &s_card);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Mount failed: %s", esp_err_to_name(ret));
		return ret;
	}

	sdmmc_card_print_info(stdout, s_card);

	return 0;
}

int sdcard_write(const char *path, const void *data, size_t len)
{
	char fullpath[64];
	snprintf(fullpath, sizeof(fullpath), "%s/%s", MOUNT_POINT, path);

	FILE *f = fopen(fullpath, "wb");
	if (!f) {
		ESP_LOGE(TAG, "Failed to open %s for writing", fullpath);
		return -ESP_ERR_NOT_FOUND;
	}
	size_t written = fwrite(data, 1, len, f);
	fclose(f);
	return written == len ? 0 : -1;
}

int sdcard_read(const char *path, void *buf, size_t len)
{
	char fullpath[64];

	snprintf(fullpath, sizeof(fullpath), "%s/%s", MOUNT_POINT, path);

	FILE *f = fopen(fullpath, "rb");
	if (!f) {
		ESP_LOGE(TAG, "Failed to open %s for reading", fullpath);
		return -ESP_ERR_NOT_FOUND;
	}
	size_t n = fread(buf, 1, len, f);
	fclose(f);

	return n;
}
