
idf_component_register(
	SRCS
	"main.c"
	"system.c"
	"drivers/lcd.c"
	"drivers/gpio.c"
	"utils/queue.c"
	"drivers/sdcard.c" 
	"prdoom/am_map.c"
	"prdoom/hu_stuff.c"
	"prdoom/p_floor.c"
	"prdoom/p_tick.c"
	"prdoom/sounds.c"
	"prdoom/info.c"
	"prdoom/p_genlin.c"
	"prdoom/p_user.c"
	"prdoom/st_lib.c"
	"prdoom/lprintf.c"
	"prdoom/p_inter.c"
	"prdoom/r_bsp.c"
	"prdoom/st_stuff.c"
	"prdoom/m_argv.c"
	"prdoom/p_lights.c"
	"prdoom/r_data.c"
	"prdoom/tables.c"
	"prdoom/m_bbox.c"
	"prdoom/p_map.c"
	"prdoom/r_demo.c"
	"prdoom/v_video.c"
	"prdoom/m_cheat.c"
	"prdoom/p_maputl.c"
	"prdoom/r_draw.c"
	"prdoom/version.c"
	"prdoom/m_menu.c"
	"prdoom/p_mobj.c"
	"prdoom/r_filter.c"
	"prdoom/w_memcache.c"
	"prdoom/m_misc.c"
	"prdoom/p_plats.c"
	"prdoom/r_fps.c"
	"prdoom/w_mmap.c"
	"prdoom/m_random.c"
	"prdoom/p_pspr.c"
	"prdoom/r_main.c"
	"prdoom/w_wad.c"
	"prdoom/md5.c"
	"prdoom/p_saveg.c"
	"prdoom/r_patch.c"
	"prdoom/wi_stuff.c"
	"prdoom/mmus2mid.c"
	"prdoom/p_setup.c"
	"prdoom/r_plane.c"
	"prdoom/z_bmalloc.c"
	"prdoom/p_ceilng.c"
	"prdoom/p_sight.c"
	"prdoom/r_segs.c"
	"prdoom/z_zone.c"
	"prdoom/p_checksum.c"
	"prdoom/p_spec.c"
	"prdoom/r_sky.c"
	"prdoom/p_doors.c"
	"prdoom/p_switch.c"
	"prdoom/r_things.c"
	"prdoom/p_enemy.c"
	"prdoom/p_telept.c"
	"prdoom/s_sound.c"
	PRIV_REQUIRES spi_flash esp_lcd esp_lcd_st7796 esp_timer sdmmc fatfs esp_driver_i2c esp_pm
	INCLUDE_DIRS "")

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
