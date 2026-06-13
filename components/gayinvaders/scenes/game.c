#include "llist.h"
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

#define ENEMY_RO_LEFT  0
#define ENEMY_RO_DOWN  1
#define ENEMY_RO_RIGHT 2
#define ENEMY_RO_CNT   3

typedef struct {
	game_object_t go;
	render_obj_t ro[ENEMY_RO_CNT];
	int health;
} enemy_t;

static uint16_t *_bullet_img;

static uint16_t *_enemy_images[ENEMY_RO_CNT];

static player_t _player;

#define BULLET_SPEED      50
#define BULLETS_POOL_SIZE 50

static bullet_t _bullets[BULLETS_POOL_SIZE] = {};

#define ENEMY_SPEED      30
#define ENEMY_POOL_SIZE  20
static enemy_t _enemies[ENEMY_POOL_SIZE] = {};

static timer_handle_t *_shooting_tim;
static timer_handle_t *_enemy_spawner_tim;

static int _level = 1;
static int _enemy_formation_index = 0;
static int _enemy_formations[3][3] = {
	{1, 0, 1},
	{0, 1, 0},
	{1, 1, 1},
};

static void _on_fire_handler(void)
{
	int i;

	for (i = 0; i < BULLETS_POOL_SIZE; ++i) {
		bullet_t *b = &_bullets[i];

		if (b->go.active)
			continue;

		b->go.active = true;

		b->go.x = _player.go.x;
		b->go.y = _player.go.y - (float)_player.ro.h/2 - (float)b->ro.h/2;
		break;
	}
}

static void _shooting_timer_handler(void *tmp)
{
	_on_fire_handler();
}

static enemy_t *_get_new_enemy(void)
{
	int i;

	for (i = 0; i < ENEMY_POOL_SIZE; ++i)
		if (!_enemies[i].go.active)
			return &_enemies[i];

	return NULL;
}

static void _enemy_spawn_formation(int *formation)
{
	int i;

	for (i = 0; i < 3; ++i) {
		int start = SCREEN_W / 4;
		enemy_t *e;

		if (!formation[i])
			continue;

		e = _get_new_enemy();
		if (!e)
			continue;

		e->go.x = start + i*100;
		e->go.y = 0;
		e->go.active = true;
	}
}

static void _enemy_spawner(void *data)
{
	if (_level == 1) {
		_enemy_spawn_formation(_enemy_formations[_enemy_formation_index]);
		_enemy_formation_index++;
		if (_enemy_formation_index == 3)
			_level++;
	}
}

static void _init()
{
	const asset_info_t *ass_inf;
	int i;

	/* Set input handlers */
	inputs_set_on_handler(INPUT_FIRE, _on_fire_handler);

	/* Player init */
	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);

	_player.ro.buff = gayinvaders_malloc(ass_inf->size);
	_player.ro.parent = &_player.go;
	_player.ro.w = ass_inf->w;
	_player.ro.h = ass_inf->h;
	_player.go.type = GAME_OBJECT_TYPE_PLAYER;
	_player.go.x = (float)SCREEN_W / 2;
	_player.go.y = (float)SCREEN_H - ass_inf->h;
	_player.go.vx = -100.0;
	_player.go.vy = 0.0;
	_player.go.ax = 0.0;
	_player.go.ay = 0.0;
	_player.go.active = true;

	wd_read_asset(ASSET_TYPE_PLAYER, _player.ro.buff, 0, 0, ass_inf->w, ass_inf->h);
	physics_register(&_player.go);

	/* Bullet init */
	ass_inf = wd_get_asset_info(ASSET_TYPE_BULLET);
	_bullet_img = gayinvaders_malloc(ass_inf->size);
	wd_read_asset(ASSET_TYPE_BULLET, _bullet_img, 0, 0, ass_inf->w, ass_inf->h);

	for (i = 0; i < BULLETS_POOL_SIZE; ++i) {
		bullet_t *b = &_bullets[i];

		b->go.type = GAME_OBJECT_TYPE_BULLET;
		b->go.vy = -BULLET_SPEED;
		b->ro.parent = &b->go;
		b->ro.w = ass_inf->w;
		b->ro.h = ass_inf->h;
		b->ro.buff = _bullet_img;

		physics_register(&b->go);
	}

	/* Enemy init */

	ass_inf = wd_get_asset_info(ASSET_TYPE_PLAYER);
	for (i = 0; i < ENEMY_RO_CNT; ++i) {
		_enemy_images[i] = gayinvaders_malloc(ass_inf->size);
		wd_read_asset(ASSET_TYPE_PLAYER, _enemy_images[i], 0, 0, ass_inf->w, ass_inf->h);
	}

	for (i = 0; i < ENEMY_POOL_SIZE; ++i) {
		enemy_t *e = &_enemies[i];
		int j;

		e->go.type = GAME_OBJECT_TYPE_ENEMY;
		e->go.vy = ENEMY_SPEED;
		for (j = 0; j < ENEMY_RO_CNT; ++j) {
			e->ro[j].parent = &e->go;
			e->ro[j].w = ass_inf->w;
			e->ro[j].h = ass_inf->h;
			e->ro[j].buff = _enemy_images[j];
		}

		physics_register(&e->go);
	}


	_shooting_tim = timers_start(2000, true, NULL, _shooting_timer_handler);
	_enemy_spawner_tim = timers_start(2000, true, NULL, _enemy_spawner);
}

static void _update(float dt)
{
	int i;

	_player.go.ax = 0.0f;
	if (inputs_get(INPUT_LEFT) == INPUT_STATE_ON)
		_player.go.ax = -500.0f;

	if (inputs_get(INPUT_RIGHT) == INPUT_STATE_ON)
		_player.go.ax = 500.0f;

	if (_player.go.x >= SCREEN_W-_player.ro.w/2) 
		_player.go.vx = -_player.go.vx;
	if (_player.go.x <= 0+_player.ro.w/2) 
		_player.go.vx = -_player.go.vx;

	for (i = 0; i < BULLETS_POOL_SIZE; ++i) {
		bullet_t *b = &_bullets[i];

		if (!b->go.active)
			continue;

		/* Disable bullet if out of screen */
		if (b->go.y < 0 - b->ro.h)
			b->go.active = false;
	}

	renderer_render(&_player.ro);
	for (i = 0; i < ENEMY_POOL_SIZE; ++i)
		renderer_render(&_enemies[i].ro[1]);
	for (i = 0; i < BULLETS_POOL_SIZE; ++i)
		renderer_render(&_bullets[i].ro);
}

static void _end()
{
	timers_stop(_shooting_tim);
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
