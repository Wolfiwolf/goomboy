#include "gameobject.h"
#include "gayinvaders.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

static int _new_scene = -1;

static void _render_intro(void)
{
	renderer_stream_asset(ASSET_TYPE_INTRO);
}

static void _intro_end(void *data)
{
	_new_scene = SCENE_TYPE_MAINMENU;
	// _new_scene = SCENE_TYPE_GAME;
}

static void _init()
{
	_render_intro();
	timers_start(2000, false, NULL, _intro_end);
}

static int _change_scene(void)
{
	return _new_scene;
}

static scene_t _intro_scene = {
	.init = _init,
	.change_scene = _change_scene,
};

scene_t *scenes_get_intro_scene(void)
{
	return &_intro_scene;
}
