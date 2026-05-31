
idf_component_register(
	SRCS
	"main.c"
	"system.c"
	"drivers/lcd.c"
	"drivers/gpio.c"
	"utils/queue.c"
	"drivers/sdcard.c" 
	#PRIV_REQUIRES spi_flash esp_lcd esp_lcd_st7796 esp_timer sdmmc fatfs esp_driver_i2c esp_pm prboom prboom-esp32-compat
	PRIV_REQUIRES esp_lcd_st7796 esp_timer sdmmc fatfs esp_driver_i2c doomgeneric
	INCLUDE_DIRS "../components/doomgeneric")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-dangling-pointer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-parameter)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-misleading-indentation)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-variable)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-variable)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-bool-compare)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-pointer-sign)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-switch)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-dangling-else)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-old-style-definition)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-implicit-fallthrough)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-compare-distinct-pointer-types)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-function)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-incompatible-pointer-types)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-parentheses)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-implicit-function-declaration)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-value)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-comment)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-maybe-uninitialized)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wmissing-field-initializers)

