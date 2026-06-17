#include "gpio.h"

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "PCAL9535A"

/*
 * NXP PCAL9535A - low-voltage 16-bit I2C GPIO expander (Agile I/O).
 *
 * 7-bit slave address is  0 1 0 0 A2 A1 A0  ->  0x20 | (A2<<2 | A1<<1 | A0).
 * Valid range is 0x20..0x27. Set PCAL_ADDR_PINS to match the A0/A1/A2 strap
 * pins on the board. (The previous 0x02 here was wrong - it dropped the fixed
 * 0b0100 prefix.)
 */
#define PCAL_ADDR_PINS  0b000                  /* A2 A1 A0 strapping */
#define I2C_DEV_ADDR    (0b0100000 | PCAL_ADDR_PINS)

#define I2C_PORT   I2C_NUM_0
#define I2C_SCL    25
#define I2C_SDA    32
#define I2C_CLK_HZ 100000

/* Command-byte (register) map. Registers come in port-0 / port-1 pairs. */
#define REG_INPUT_0       0x00   /* read-only: live pin levels            */
#define REG_INPUT_1       0x01
#define REG_OUTPUT_0      0x02   /* output latch (POR 0xFF)               */
#define REG_OUTPUT_1      0x03
#define REG_POL_INV_0     0x04   /* 1 = invert this input in INPUT reg    */
#define REG_POL_INV_1     0x05
#define REG_CONFIG_0      0x06   /* 1 = input, 0 = output (POR 0xFF)      */
#define REG_CONFIG_1      0x07
#define REG_DRIVE_0A      0x40   /* output drive strength (2 bits/pin)    */
#define REG_DRIVE_0B      0x41
#define REG_DRIVE_1A      0x42
#define REG_DRIVE_1B      0x43
#define REG_INPUT_LATCH_0 0x44   /* 1 = latch input until read            */
#define REG_INPUT_LATCH_1 0x45
#define REG_PULL_EN_0     0x46   /* 1 = enable pull resistor              */
#define REG_PULL_EN_1     0x47
#define REG_PULL_SEL_0    0x48   /* 1 = pull-up, 0 = pull-down (POR 0xFF) */
#define REG_PULL_SEL_1    0x49
#define REG_INT_MASK_0    0x4A   /* 1 = mask interrupt (POR 0xFF)         */
#define REG_INT_MASK_1    0x4B
#define REG_INT_STAT_0    0x4C   /* read-only: 1 = pin caused interrupt   */
#define REG_INT_STAT_1    0x4D
#define REG_OUT_CONFIG    0x4F   /* ODEN1,ODEN0: 1 = open-drain port      */

/*
 * Buttons connect each input pin to GND and rely on the expander's internal
 * pull-ups, so an idle pin reads 1 and a pressed pin reads 0. The polarity
 * inversion registers flip that so gpio_read() reports 1 = pressed.
 *
 * If your buttons are instead wired to VDD (active-high), set BUTTON_MASK to 0
 * (no inversion) and switch the pulls to pull-down (REG_PULL_SEL_* = 0).
 */
#define BUTTON_MASK   0x03FF   /* pins 0..9 (see GPIO_BTN_* in gpio.h)     */

static i2c_master_bus_handle_t _bus;
static i2c_master_dev_handle_t _dev;

static int _write_reg(uint8_t reg, uint8_t val)
{
	uint8_t buf[2] = {reg, val};

	return i2c_master_transmit(_dev, buf, sizeof(buf), -1);
}

/* Write a 16-bit value across a register pair: low byte -> reg0 (port 0),
 * high byte -> reg0+1 (port 1). */
static int _write_pair(uint8_t reg0, uint16_t val)
{
	int ret = _write_reg(reg0, val & 0xFF);

	if (ret)
		return ret;
	return _write_reg(reg0 + 1, val >> 8);
}

static int _read_reg(uint8_t reg, uint8_t *val)
{
	return i2c_master_transmit_receive(_dev, &reg, 1, val, 1, -1);
}

int gpio_init(void)
{
	i2c_master_bus_config_t bus_cfg = {
	    .i2c_port = I2C_PORT,
	    .scl_io_num = I2C_SCL,
	    .sda_io_num = I2C_SDA,
	    .clk_source = I2C_CLK_SRC_DEFAULT,
	    .glitch_ignore_cnt = 7,
	    .flags.enable_internal_pullup = true,
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

	ret = i2c_master_bus_add_device(_bus, &dev_cfg, &_dev);
	if (ret)
		return ret;

	ret = i2c_master_probe(_bus, I2C_DEV_ADDR, 100);
	if (ret)
		return ret;

	/* All 16 pins as inputs (POR default, but set it explicitly). */
	ret = _write_pair(REG_CONFIG_0, 0xFFFF);
	if (ret)
		return ret;

	/* Select pull-ups (1) and enable them on every pin so no input floats. */
	ret = _write_pair(REG_PULL_SEL_0, 0xFFFF);
	if (ret)
		return ret;
	ret = _write_pair(REG_PULL_EN_0, 0xFFFF);
	if (ret)
		return ret;

	/* Invert the button inputs so a press (pin pulled low) reads as 1. */
	ret = _write_pair(REG_POL_INV_0, BUTTON_MASK);
	if (ret)
		return ret;

	return 0;
}

int gpio_read(uint16_t *bitmap)
{
	uint8_t lo, hi;
	int ret;

	ret = _read_reg(REG_INPUT_0, &lo);
	if (ret)
		return ret;

	ret = _read_reg(REG_INPUT_1, &hi);
	if (ret)
		return ret;

	*bitmap = ((uint16_t)hi << 8) | lo;
	return 0;
}

int gpio_set_dir(uint16_t input_mask)
{
	/* 1 = input (high-impedance), 0 = output. */
	return _write_pair(REG_CONFIG_0, input_mask);
}

int gpio_write(uint16_t bitmap)
{
	/* Drives only the pins configured as outputs; bits for input pins are
	 * latched but take effect only once the pin is made an output. */
	return _write_pair(REG_OUTPUT_0, bitmap);
}
