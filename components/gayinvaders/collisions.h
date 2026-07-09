#ifndef COLLISIONS_H_
#define COLLISIONS_H_

#include "boss.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "powerup.h"

void collisions_init(player_t *player,
		     bullet_t *bullets, int bullets_cnt,
		     enemy_t *enemies, int enemies_cnt,
		     powerup_t *powerups, int powerups_cnt,
		     boss_t *boss,
		     void (*on_collision)(void *obj1, game_object_type_t type1, void *obj2, game_object_type_t type2));

void collision_update(void);

#endif
