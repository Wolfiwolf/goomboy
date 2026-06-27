#include "gameobject.h"
#include "gayinvaders.h"
#include "inputs.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

typedef struct {
	game_object_t go;
	render_obj_t ro;

} button_t;

#define BUTTONS_CNT 3

static uint16_t *_selector_image = NULL;
static button_t _selector = {};

static uint16_t *_button_images[BUTTONS_CNT] = {};
static button_t _buttons[BUTTONS_CNT] = {};
static int _selected_btn = 0;

static int _new_scene = -1;

static void _render_state(void)
{
	int i;

	renderer_clear();

	for (i = 0; i < BUTTONS_CNT; ++i)
		renderer_render(&_buttons[i].ro);

	_selector.go.x = (float)SCREEN_W/5;
	_selector.go.y = ((float)SCREEN_H/4)*(_selected_btn+1);
	_selector.go.active = true;

	renderer_flip_next();
	renderer_render(&_selector.ro);

	_selector.go.x = (float)(SCREEN_W*4)/5;
	renderer_render(&_selector.ro);

	renderer_flush();
}

static void _on_down_press()
{
	_selected_btn = (_selected_btn+1) % BUTTONS_CNT;
	_render_state();
}

static void _on_up_press()
{
	_selected_btn = _selected_btn-1;

	if (_selected_btn < 0)
		_selected_btn = BUTTONS_CNT-1;
	_render_state();
}

static void _on_fire_press()
{
	switch (_selected_btn) {
	case 0:
		_new_scene = SCENE_TYPE_GAME;
		break;
	}
}

static void _init()
{
	const asset_info_t *ass_inf;
	asset_type_t ass_type;
	int i;

	inputs_set_on_handler(INPUT_DOWN, _on_down_press);
	inputs_set_on_handler(INPUT_UP, _on_up_press);
	inputs_set_on_handler(INPUT_FIRE_NORMAL, _on_fire_press);

	for (i = 0; i < BUTTONS_CNT; ++i) {
		ass_type = ASSET_TYPE_MAINMENUBTN0+i;
		ass_inf = wd_get_asset_info(ass_type);

		_buttons[i].ro.buff = wd_get_asset(ass_type);
		_buttons[i].ro.h = ass_inf->h;
		_buttons[i].ro.w = ass_inf->w;
		_buttons[i].ro.parent = &_buttons[i].go;

		_buttons[i].go.x = (float)SCREEN_W/2;
		_buttons[i].go.y = ((float)SCREEN_H/4)*(i+1);
		_buttons[i].go.active = true;
	}

	ass_type = ASSET_TYPE_MAINMENUSELECTOR;
	ass_inf = wd_get_asset_info(ass_type);

	_selector.ro.buff = wd_get_asset(ass_type);
	_selector.ro.h = ass_inf->h;
	_selector.ro.w = ass_inf->w;
	_selector.ro.parent = &_selector.go;

	_render_state();
}

static void _end(void)
{
	int i;

	for (i = 0; i < BUTTONS_CNT; ++i)
		wd_not_using(ASSET_TYPE_MAINMENUBTN0+i);

	wd_not_using(ASSET_TYPE_MAINMENUSELECTOR);
}

static int _change_scene(void)
{
	int tmp = _new_scene;
	_new_scene = -1;
	return tmp;
}

static scene_t _intro_scene = {
	.init = _init,
	.change_scene = _change_scene,
	.end = _end,
};

scene_t *scenes_get_mainmenu_scene(void)
{
	return &_intro_scene;
}

