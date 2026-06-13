#include <stdint.h>
#include "gayinvaders.h"
#include "inputs.h"
#include "physics.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

static float _prev_t = 0;

void gayinvaders_main(int argc, char *argv[])
{
	scene_t *scenes[SCENE_TYPE_COUNT] = {
		scenes_get_intro_scene(),
		scenes_get_game_scene(),
	};
	int active_scene = 0;
	int new_scene;
	float dt;
	int ret;

	renderer_init();

	ret = wd_init(argv[1]);
	if (ret)
		return;


	physics_init();

	renderer_clear();

	scenes[active_scene]->init();
	
	_prev_t = gayinvaders_get_ms();

	for (;;) {

		dt = ((float)gayinvaders_get_ms() - (float)_prev_t) / 1000.0f;
		_prev_t = gayinvaders_get_ms();

		inputs_update();

		physics_update(dt);
		timers_update(dt);

		renderer_clear();

		scenes[active_scene]->update(dt);

		renderer_flush();
		gayinvaders_sleep_ms(15);

		new_scene = scenes[active_scene]->change_scene();
		if (new_scene > 0) {
			scenes[active_scene]->end();
			active_scene = new_scene;
			scenes[active_scene]->init();
		}
	}
}

void gayinvaders_render_finished(void)
{
	renderer_buffer_unlock();
}
