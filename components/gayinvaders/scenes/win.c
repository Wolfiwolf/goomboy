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
	renderer_stream_asset(ASSET_TYPE_WIN);
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
