#include "gameobject.h"
#include "gayinvaders.h"
#include "inputs.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

static int _new_scene = -1;

static void _render(void)
{
	renderer_stream_asset(ASSET_TYPE_BADBOYSTANK);
}

static void _on_rapidfire_press()
{
	_new_scene = SCENE_TYPE_BADBOYS;
}

static void _init()
{
	inputs_set_on_handler(INPUT_RAPIDFIRE, _on_rapidfire_press);
	_render();
}

static int _change_scene(void)
{
	int tmp = _new_scene;
	_new_scene = -1;
	return tmp;
}

static scene_t _badboystank_scene = {
	.init = _init,
	.change_scene = _change_scene,
};

scene_t *scenes_get_badboystank_scene(void)
{
	return &_badboystank_scene;
}
