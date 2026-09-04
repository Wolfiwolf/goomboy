#include "gameobject.h"
#include "gayinvaders.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

static int _new_scene = -1;

static void _render_dead(void)
{
	renderer_stream_asset(ASSET_TYPE_DEAD);
}

static void _dead_end(void *data)
{
	_new_scene = SCENE_TYPE_GAME;
}

static void _init()
{
	_render_dead();
	timers_start(2000, false, NULL, _dead_end);
}

static int _change_scene(void)
{
	int tmp = _new_scene;
	_new_scene = -1;
	return tmp;
}

static scene_t _dead_scene = {
	.init = _init,
	.change_scene = _change_scene,
};

scene_t *scenes_get_dead_scene(void)
{
	return &_dead_scene;
}
