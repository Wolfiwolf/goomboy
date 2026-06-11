#include "scene.h"

#include <stdio.h>
#include <stdint.h>
#include "gayinvaders.h"
#include "gameobject.h"
#include "inputs.h"
#include "physics.h"
#include "renderer.h"
#include "timers.h"
#include "wd.h"

static int _new_scene = -1;

typedef struct {
	game_object_t go;
	render_obj_t ro;
} player_t;

typedef struct {
	game_object_t go;
	render_obj_t ro;
} bullet_t;

static uint16_t *_bullet_img;
static const asset_info_t *_bullet_asset_info;

static player_t _player = { };

static bullet_t *_bull = NULL;

static void _on_fire_handler(void)
{
	const asset_info_t *ass;
	bullet_t *bull;

	if (_bull) {
		_bull->go.x = _player.go.x;
		_bull->go.y = _player.go.y - (float)_player.ro.h/2 - (float)_bull->ro.h/2;
		return;
	}

	ass = wd_get_asset_info(ASSET_TYPE_BULLET);

	bull = gayinvaders_malloc(sizeof(bullet_t));
	bull->ro.parent = &bull->go;
	bull->ro.buff = _bullet_img;
	bull->ro.w = ass->w;
	bull->ro.h = ass->h;
	bull->go.x = _player.go.x;
	bull->go.y = _player.go.y - (float)_player.ro.h/2 - (float)bull->ro.h/2;
	bull->go.vx = 0.0;
	bull->go.vy = -100.0;
	bull->go.ax = 0.0;
	bull->go.ay = 0.0;
	bull->go.resistance = 0.0;
	bull->go.active = true;

	physics_register(&bull->go);

	_bull = bull;
}

static void _timer_end(void *tmp)
{
	_on_fire_handler();
	timers_start(4000, NULL, _timer_end);
}

static void _init()
{
	const asset_info_t *ass_inf, *bull_ass_inf;

	inputs_set_on_handler(INPUT_FIRE, _on_fire_handler);

	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);
	bull_ass_inf = wd_get_asset_info(ASSET_TYPE_BULLET);

	_bullet_img = gayinvaders_malloc(bull_ass_inf->size);
	wd_read_asset(ASSET_TYPE_BULLET, _bullet_img, 0, 0, bull_ass_inf->w, bull_ass_inf->h);

	_player.ro.buff = gayinvaders_malloc(ass_inf->size);
	_player.ro.parent = &_player.go;
	_player.ro.w = ass_inf->w;
	_player.ro.h = ass_inf->h;
	_player.go.x = (float)SCREEN_W / 2;
	_player.go.y = (float)SCREEN_H - ass_inf->h;
	_player.go.vx = -100.0;
	_player.go.vy = 0.0;
	_player.go.ax = 0.0;
	_player.go.ay = 0.0;
	_player.go.active = true;

	wd_read_asset(ASSET_TYPE_PLAYER, _player.ro.buff, 0, 0, ass_inf->w, ass_inf->h);
	physics_register(&_player.go);

	timers_start(2000, NULL, _timer_end);
}

static void _update(float dt)
{
	_player.go.ax = 0.0f;
	if (inputs_get(INPUT_LEFT) == INPUT_STATE_ON)
		_player.go.ax = -500.0f;

	if (inputs_get(INPUT_RIGHT) == INPUT_STATE_ON)
		_player.go.ax = 500.0f;

	if (_player.go.x >= SCREEN_W-_player.ro.w/2) 
		_player.go.vx = -_player.go.vx;
	if (_player.go.x <= 0+_player.ro.w/2) 
		_player.go.vx = -_player.go.vx;


	renderer_render(&_player.ro);
	if (_bull)
		renderer_render(&_bull->ro);
}

static void _end()
{

}

static int _change_scene(void)
{
	return _new_scene;
}

static scene_t _game_scene = {
	.init = _init,
	.update = _update,
	.end = _end,
	.change_scene = _change_scene,
};

scene_t *scenes_get_game_scene(void)
{
	return &_game_scene;
}
