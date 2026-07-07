#include "bullet.h"
#include "collisions.h"
#include "enemy.h"
#include "hud.h"
#include "number.h"
#include "player.h"
#include "powerup.h"
#include "scene.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gayinvaders.h"
#include "gameobject.h"
#include "inputs.h"
#include "physics.h"
#include "renderer.h"
#include "timers.h"

static int _new_scene = -1;

#define PLAYER_SPEED 100

static player_t _player;
static hud_t _hud;

#define BULLETS_POOL_SIZE 50
static bullet_t _bullets[BULLETS_POOL_SIZE] = {};

#define POWERUP_POOL_SIZE 10
static powerup_t _powerups[POWERUP_POOL_SIZE] = {};

#define ENEMY_POOL_SIZE  15
static enemy_t _enemies[ENEMY_POOL_SIZE] = {};

static timer_handle_t *_enemy_spawner_tim = NULL;
static timer_handle_t *_powerup_spawner_tim = NULL;

#define ENEMY_SPAWN_INTERVAL 5000
#define ENEMY_MIN_SPAWN_INTERVAL 1500
static int _level = 0;
static number_t _level_num;

static bool _player_killed_triggered = false;

static bool _is_boss_fight = false;

static void _on_fire_normal_handler(void)
{
	player_fire(&_player, BULLET_TYPE_NORMAL, _bullets, BULLETS_POOL_SIZE);
}

static void _on_fire_bomb_handler(void)
{
	player_fire(&_player, BULLET_TYPE_BOMB, _bullets, BULLETS_POOL_SIZE);
}

static void _on_shield_handler(void)
{
	player_shield_up(&_player);
}

static void _on_rapidfire_handler(void)
{
	player_rapidfire_on(&_player);
}

static void _on_select_handler(void)
{
	_new_scene = SCENE_TYPE_MAINMENU;
}

static enemy_t *_get_new_enemy(void)
{
	int i;

	for (i = 0; i < ENEMY_POOL_SIZE; ++i)
		if (!_enemies[i].go.active)
			return &_enemies[i];

	return NULL;
}

static powerup_t *_get_new_powerup(void)
{
	int i;

	for (i = 0; i < POWERUP_POOL_SIZE; ++i)
		if (!_powerups[i].go.active)
			return &_powerups[i];

	return NULL;
}

static void _start_boss_fight(void)
{
	_is_boss_fight = true;
}

static void _enemy_spawner(void *data)
{
	enemy_type_t maxtype;
	int spawn_interval;
	enemy_t *e;

	if (_is_boss_fight)
		return;

	e = _get_new_enemy();

	if (!e)
		return;

	if (_level < 2)
		maxtype = ENEMY_TYPE_EASY;
	else if (_level < 4)
		maxtype = ENEMY_TYPE_MOVING;
	else if (_level < 10)
		maxtype = ENEMY_TYPE_FAST;
	else if (_level < 20)
		maxtype = ENEMY_TYPE_TANK;
	else 
		maxtype = ENEMY_TYPE_BOSS;

	if (maxtype == ENEMY_TYPE_BOSS) {
		_start_boss_fight();
		return;
	}

	if (_level > 18)
		enemy_activate(e, ENEMY_TYPE_TANK, -1, -e->images[0].h);
	else
		enemy_activate(e, rand() % (maxtype+1), -1, -e->images[0].h);

	_level += 1;
	number_set_val(&_level_num, _level);

	spawn_interval = ENEMY_SPAWN_INTERVAL - (_level * 250);

	if (spawn_interval < ENEMY_MIN_SPAWN_INTERVAL)
		spawn_interval = ENEMY_MIN_SPAWN_INTERVAL;

	timers_change_dur(_enemy_spawner_tim, spawn_interval);
}

static void _powerup_spawner(void *data)
{
	powerup_t *pu;

	pu = _get_new_powerup();
	if (!pu)
		return;

	powerup_activate(pu, rand() % POWERUP_TYPE_CNT,
			(rand() % (SCREEN_W - pu->ro.w))+(pu->ro.w/2),
			 -pu->ro.h);
}

static void _end_game(void *data)
{
	_new_scene = SCENE_TYPE_DEAD;
}

static void _kill_enemy(void *enemy)
{
	enemy_diactivate(enemy);
}

static void _player_killed(void)
{
	if (_player_killed_triggered)
		return;

	_player_killed_triggered = true;

	if (_enemy_spawner_tim)
		timers_stop(_enemy_spawner_tim);
	if (_powerup_spawner_tim)
		timers_stop(_powerup_spawner_tim);

	_enemy_spawner_tim = NULL;
	_powerup_spawner_tim = NULL;

	timers_start(2000, false, NULL, _end_game);
}

static void _on_collision(void *obj1, game_object_type_t type1, void *obj2, game_object_type_t type2)
{
	if (type1 == GAME_OBJECT_TYPE_ENEMY) {
		bullet_t *b = obj2;
		enemy_t *e = obj1;

		enemy_damage(e, b->damage);
		bullet_hit(b);
	}

	if (type1 == GAME_OBJECT_TYPE_PLAYER) {
		if (type2 == GAME_OBJECT_TYPE_BULLET) {
			bullet_t *b = obj2;
			player_damage(&_player, b->damage);
			bullet_hit(b);
		} else if (type2 == GAME_OBJECT_TYPE_POWERUP) {
			powerup_t *pu = obj2;

			if (pu->type == POWERUP_TYPE_HEALTH) {
				if (_player.health != PLAYER_MAX_HEALTH)
					_player.health += 1;
			} else if (pu->type == POWERUP_TYPE_BOMB) {
				_player.has_bomb = true;
			} else if (pu->type == POWERUP_TYPE_SHIELD) {
				_player.has_shield = true;
			} else if (pu->type == POWERUP_TYPE_RAPIDFIRE) {
				_player.has_rapidfire = true;
			}

			powerup_diactivate(pu);
		}
	}
}

static void _init()
{
	int i;

	/* Set input handlers */
	inputs_set_on_handler(INPUT_FIRE_NORMAL, _on_fire_normal_handler);
	inputs_set_on_handler(INPUT_FIRE_BOMB, _on_fire_bomb_handler);
	inputs_set_on_handler(INPUT_SHIELD, _on_shield_handler);
	inputs_set_on_handler(INPUT_RAPIDFIRE, _on_rapidfire_handler);
	inputs_set_on_handler(INPUT_SELECT, _on_select_handler);

	player_init(&_player, SCREEN_W / 4, SCREEN_H-32);

	memset(_bullets, 0, sizeof(_bullets));
	for (i = 0; i < BULLETS_POOL_SIZE; ++i)
		bullet_init(&_bullets[i]);

	memset(_enemies, 0, sizeof(_enemies));
	for (i = 0; i < ENEMY_POOL_SIZE; ++i)
		enemy_init(&_enemies[i]);

	memset(_powerups, 0, sizeof(_powerups));
	for (i = 0; i < POWERUP_POOL_SIZE; ++i)
		powerup_init(&_powerups[i]);

	hud_init(&_hud, 50, SCREEN_H - 8);

	number_init(&_level_num, 20, SCREEN_H-30);

	collisions_init(&_player,
			_bullets, BULLETS_POOL_SIZE,
			_enemies, ENEMY_POOL_SIZE,
			_powerups, POWERUP_POOL_SIZE,
			_on_collision);

	_enemy_spawner_tim = timers_start(6000, true, NULL, _enemy_spawner);
	_powerup_spawner_tim = timers_start(7000, true, NULL, _powerup_spawner);

	_level = 0;
	number_set_val(&_level_num, _level);
	
	_player_killed_triggered = false;
}

static void _update(float dt)
{
	int i;

	// Inputs
	if (!_player.dead) {
		player_go_stop(&_player);
		if (inputs_get(INPUT_LEFT) == INPUT_STATE_ON)
			player_go_left(&_player);
		if (inputs_get(INPUT_RIGHT) == INPUT_STATE_ON)
			player_go_right(&_player);
	}

	player_update(&_player, dt, _bullets, BULLETS_POOL_SIZE);

	if (_player.dead) {
		_player_killed();
	} else {
		physics_update(&_player.go, dt);
		for (i = 0; i < ENEMY_POOL_SIZE; ++i) {
			enemy_t *e = &_enemies[i];
			if (!e->go.active)
				continue;

			enemy_update(e, dt, _bullets, BULLETS_POOL_SIZE, &_player);
		}
		for (i = 0; i < BULLETS_POOL_SIZE; ++i)
			bullet_update(&_bullets[i], dt);
		for (i = 0; i < POWERUP_POOL_SIZE; ++i)
			powerup_update(&_powerups[i], dt);

		collision_update();
	}

	hud_update(&_hud, _player.health, _player.has_bomb,
		   _player.has_shield, _player.has_rapidfire);
}

static void _render(void)
{
	int i;

	for (i = 0; i < ENEMY_POOL_SIZE; ++i)
		enemy_render(&_enemies[i]);

	player_render(&_player);

	for (i = 0; i < BULLETS_POOL_SIZE; ++i)
		renderer_render(&_bullets[i].ro);
	for (i = 0; i < POWERUP_POOL_SIZE; ++i)
		renderer_render(&_powerups[i].ro);

	number_render(&_level_num);

	hud_render(&_hud);
}

static void _end()
{
	int i;

	/* Unset input handlers */
	inputs_set_on_handler(INPUT_FIRE_NORMAL, NULL);
	inputs_set_on_handler(INPUT_FIRE_BOMB, NULL);
	inputs_set_on_handler(INPUT_SHIELD, NULL);
	inputs_set_on_handler(INPUT_RAPIDFIRE, NULL);
	inputs_set_on_handler(INPUT_SELECT, NULL);

	if (_enemy_spawner_tim)
		timers_stop(_enemy_spawner_tim);
	if (_powerup_spawner_tim)
		timers_stop(_powerup_spawner_tim);
	
	for (i = 0; i < BULLETS_POOL_SIZE; ++i) {
		bullet_t *b = &_bullets[i];

		bullet_diactivate(b);
		bullet_destroy(b);
	}

	for (i = 0; i < POWERUP_POOL_SIZE; ++i) {
		powerup_t *pu = &_powerups[i];

		powerup_diactivate(pu);
		powerup_destroy(pu);
	}

	for (i = 0; i < ENEMY_POOL_SIZE; ++i) {
		enemy_t *e = &_enemies[i];

		enemy_diactivate(e);
		enemy_destroy(e);
	}

	number_destroy(&_level_num);

	hud_destroy(&_hud);

	player_destroy(&_player);
}

static int _change_scene(void)
{
	int tmp = _new_scene;
	_new_scene = -1;
	return tmp;
}

static scene_t _game_scene = {
	.init = _init,
	.update = _update,
	.render = _render,
	.end = _end,
	.change_scene = _change_scene,
};

scene_t *scenes_get_game_scene(void)
{
	return &_game_scene;
}
