#include "gpio.h"

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#define TAG "I2C"

#define I2C_DEV_ADDR 0x00

#define I2C_PORT I2C_NUM_0
#define I2C_SCL 25
#define I2C_SDA 32
#define I2C_CLK_HZ 400000

#define REG_IN_PORT0 0x00
#define REG_IN_PORT1 0x01

static i2c_master_bus_handle_t _bus;
static i2c_master_dev_handle_t _dev;

static int _write_reg(uint8_t reg, uint8_t val)
{
	uint8_t buf[2] = {reg, val};

	return i2c_master_transmit(_dev, buf, sizeof(buf), 100);
}

static int _read_reg(uint8_t reg, uint8_t *val)
{
	return i2c_master_transmit_receive(_dev, &reg, 1, val, 1, 100);
}

int gpio_init(void)
{
	i2c_master_bus_config_t bus_cfg = {
	    .i2c_port = I2C_PORT,
	    .scl_io_num = I2C_SCL,
	    .sda_io_num = I2C_SDA,
	    .clk_source = I2C_CLK_SRC_DEFAULT,
	    .glitch_ignore_cnt = 7,
	};
	i2c_device_config_t dev_cfg = {
	    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
	    .device_address = I2C_DEV_ADDR,
	    .scl_speed_hz = I2C_CLK_HZ,
	};
	int ret;

	ret = i2c_new_master_bus(&bus_cfg, &_bus);
	if (ret)
		return ret;

	return i2c_master_bus_add_device(_bus, &dev_cfg, &_dev);
}

int gpio_read(uint16_t *bitmap)
{
	int ret;

	ret = _read_reg(REG_IN_PORT0, (uint8_t*)bitmap);
	if (ret)
		return ret;
	return _read_reg(REG_IN_PORT1, ((uint8_t*)bitmap) + 1);
}
