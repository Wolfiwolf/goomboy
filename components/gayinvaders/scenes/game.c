#include "bullet.h"
#include "collisions.h"
#include "enemy.h"
#include "hud.h"
#include "llist.h"
#include "player.h"
#include "powerup.h"
#include "scene.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gayinvaders.h"
#include "gameobject.h"
#include "inputs.h"
#include "physics.h"
#include "renderer.h"
#include "timers.h"
#include "wd.h"

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

static int _level = 1;
static int _enemy_formation_index = 0;
static int _enemy_formations[3][3] = {
	{0, 1, 0},
	{0, 1, 0},
	{0, 1, 0},
};

static bool _player_killed_triggered = false;

static bool _is_boss_fight = false;

static void _on_fire_normal_handler(void)
{
	player_fire(&_player, BULLET_TYPE_NORMAL, _bullets, BULLETS_POOL_SIZE);
}

static void _on_fire_bomb_handler(void)
{
	if (_player.ammo[BULLET_TYPE_BOMB - BULLET_PLAYER_SPECIAL_START])
		player_fire(&_player, BULLET_TYPE_BOMB, _bullets, BULLETS_POOL_SIZE);
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
	enemy_t *e = _get_new_enemy();
	enemy_type_t maxtype;

	if (_is_boss_fight)
		return;

	if (!e)
		return;

	if (_level < 2)
		maxtype = ENEMY_TYPE_EASY;
	else if (_level < 3)
		maxtype = ENEMY_TYPE_MOVING;
	else if (_level < 50)
		maxtype = ENEMY_TYPE_FAST;
	else if (_level < 20)
		maxtype = ENEMY_TYPE_TANK;
	else 
		maxtype = ENEMY_TYPE_BOSS;

	if (maxtype != ENEMY_TYPE_BOSS) {
		enemy_activate(e, rand() % (maxtype+1), -1, -e->images[0].h);
		_level += 1;
	} else {
		_start_boss_fight();
	}
}

static void _powerup_spawner(void *data)
{
	powerup_t *pu;

	pu = _get_new_powerup();
	if (!pu)
		return;

	powerup_activate(pu, rand() % POWERUP_TYPE_CNT, SCREEN_W_HALF, -pu->ro.h);
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

static void _shoot_bomb(void *data)
{
	_on_fire_bomb_handler();
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
			_player.health -= b->damage;
			bullet_hit(b);
		} else if (type2 == GAME_OBJECT_TYPE_POWERUP) {
			powerup_t *pu = obj2;

			if (pu->type == POWERUP_TYPE_HEALTH) {
				if (_player.health != PLAYER_MAX_HEALTH)
					_player.health += 1;
			} else if (pu->type == POWERUP_TYPE_BOMB) {
				_player.ammo[BULLET_TYPE_BOMB - BULLET_PLAYER_SPECIAL_START] += 1;
				timers_start(1000, false, NULL, _shoot_bomb);
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

	collisions_init(&_player,
			_bullets, BULLETS_POOL_SIZE,
			_enemies, ENEMY_POOL_SIZE,
			_powerups, POWERUP_POOL_SIZE,
			_on_collision);

	_enemy_spawner_tim = timers_start(6000, true, NULL, _enemy_spawner);
	_powerup_spawner_tim = timers_start(5000, true, NULL, _powerup_spawner);

	_level = 1;
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

	player_update(&_player, dt);

	if (_player.dead) {
		_player_killed();
	} else {
		physics_update(&_player.go, dt);
		for (i = 0; i < ENEMY_POOL_SIZE; ++i) {
			enemy_t *e = &_enemies[i];
			if (!e->go.active)
				continue;

			enemy_update(e, dt, _bullets, BULLETS_POOL_SIZE, &_player.go);
			if (e->go.y >= SCREEN_H- (float)e->images[0].h/2) {
				_player.health -= 3;
				enemy_kill(e);
			}
		}
		for (i = 0; i < BULLETS_POOL_SIZE; ++i)
			bullet_update(&_bullets[i], dt);
		for (i = 0; i < POWERUP_POOL_SIZE; ++i)
			powerup_update(&_powerups[i], dt);

		collision_update();
	}

	hud_update(&_hud, _player.health, _player.ammo[0]);
}

static void _render(void)
{
	int i;

	for (i = 0; i < ENEMY_POOL_SIZE; ++i)
		enemy_render(&_enemies[i]);

	renderer_render(&_player.ro);

	for (i = 0; i < BULLETS_POOL_SIZE; ++i)
		renderer_render(&_bullets[i].ro);
	for (i = 0; i < POWERUP_POOL_SIZE; ++i)
		renderer_render(&_powerups[i].ro);

	hud_render(&_hud);
}

static void _end()
{
	int i;
	
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
