#include "gameobject.h"
#include "gayinvaders.h"
#include "inputs.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

static int _new_scene = -1;

static void _render_win(void)
{
	const asset_info_t *ass = wd_get_asset_info(ASSET_TYPE_WIN);
	game_object_t go;
	render_obj_t ro;
	uint16_t *buff;
	int x, y;

	ro.w = ass->w;
	ro.h = 16;
	ro.parent = &go;

	buff = gayinvaders_malloc(ro.w*ro.h*2);
	while (!buff) {
		gayinvaders_sleep_ms(10);
		buff = gayinvaders_malloc(ro.w*ro.h*2);
	}

	renderer_clear();
	for (x = 0; x < ass->w; x += ro.w) {
		for (y = 0; y < ass->h; y += ro.h) {
			go.x = x+(float)ro.w/2;
			go.y = y+(float)ro.h/2;
			wd_read_asset_direct(ASSET_TYPE_WIN, buff, x, y, ro.w, ro.h);
			ro.buff = buff;
			renderer_render(&ro);
		}
	}

	gayinvaders_free(buff);

	renderer_flush();
}

static void _win_end(void *data)
{
	_new_scene = SCENE_TYPE_GAME;
}

static void _go_to_mainmenu(void)
{
	_new_scene = SCENE_TYPE_MAINMENU;
}

static void _init()
{
	inputs_set_on_handler(INPUT_FIRE_NORMAL, _go_to_mainmenu);
	_render_win();
}

static void _end()
{
	inputs_set_on_handler(INPUT_FIRE_NORMAL, NULL);
}

static int _change_scene(void)
{
	int tmp = _new_scene;
	_new_scene = -1;
	return tmp;
}

static scene_t _win_scene = {
	.init = _init,
	.end  = _end,
	.change_scene = _change_scene,
};

scene_t *scenes_get_win_scene(void)
{
	return &_win_scene;
}
