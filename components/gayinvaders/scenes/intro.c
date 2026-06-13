#include "gameobject.h"
#include "gayinvaders.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"
#include <stdio.h>

static int _new_scene = -1;

static void _render_intro(void)
{
	const asset_info_t *ass = wd_get_asset_info(ASSET_TYPE_INTRO);
	game_object_t go;
	render_obj_t ro;
	int x, y;


	ro.w = ass->w;
	ro.h = 16;
	ro.parent = &go;

	ro.buff = gayinvaders_malloc(ro.w*ro.h*2);

	renderer_clear();
	for (x = 0; x < ass->w; x += ro.w) {
		for (y = 0; y < ass->h; y += ro.h) {
			go.x = x+(float)ro.w/2;
			go.y = y+(float)ro.h/2;
			wd_read_asset(ASSET_TYPE_INTRO, ro.buff, x, y, ro.w, ro.h);
			renderer_render(&ro);
		}
	}

	gayinvaders_free(ro.buff);

	renderer_flush();
}

static void _intro_end(void *data)
{
	_new_scene = SCENE_TYPE_GAME;
}

static void _init()
{
	_render_intro();
	timers_start(2000, false, NULL, _intro_end);
}

static void _update(float dt)
{
	renderer_dont_flush();
}

static void _end()
{

}

static int _change_scene(void)
{
	return _new_scene;
}

static scene_t _intro_scene = {
	.init = _init,
	.update = _update,
	.end = _end,
	.change_scene = _change_scene,
};

scene_t *scenes_get_intro_scene(void)
{
	return &_intro_scene;
}
