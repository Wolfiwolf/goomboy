#include <stdint.h>
#include <stdio.h>
#include "gayinvaders.h"
#include "inputs.h"
#include "physics.h"
#include "renderer.h"
#include "scene.h"
#include "timers.h"
#include "wd.h"

#define FREE_MEM_DISPLAY_INTERVAL 2000

static float _prev_t = 0;
static float _prev_mem_display_t = 0;

void gayinvaders_main(int argc, char *argv[])
{
	scene_t *scenes[SCENE_TYPE_COUNT] = {
		scenes_get_intro_scene(),
		scenes_get_game_scene(),
		scenes_get_dead_scene(),
	};
	int active_scene = 0;
	int new_scene;
	int fps_cnt;
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
	_prev_mem_display_t = _prev_t;

	fps_cnt = 0;

	for (;;) {
		dt = ((float)gayinvaders_get_ms() - (float)_prev_t) / 1000.0f;
		_prev_t = gayinvaders_get_ms();

		inputs_update();

		timers_update(dt);


		if (scenes[active_scene]->update)
			scenes[active_scene]->update(dt);

		if (scenes[active_scene]->render) {
			renderer_clear();
			scenes[active_scene]->render();
			renderer_flush();
		}

		gayinvaders_sleep_ms(15);

		new_scene = scenes[active_scene]->change_scene();
		if (new_scene > 0) {
			if (scenes[active_scene]->end)
				scenes[active_scene]->end();
			active_scene = new_scene;
			if (scenes[active_scene]->init)
				scenes[active_scene]->init();
		}

		fps_cnt += 1;
		if (gayinvaders_get_ms() - _prev_mem_display_t > FREE_MEM_DISPLAY_INTERVAL) {

			printf("FPS: %.3f, Free mem: %lu\n", (float)FREE_MEM_DISPLAY_INTERVAL/(float)fps_cnt, gayinvaders_free_mem());
			_prev_mem_display_t = gayinvaders_get_ms();
			fps_cnt = 0;
		}
	}
}

void gayinvaders_render_finished(void)
{
	renderer_buffer_unlock();
}
