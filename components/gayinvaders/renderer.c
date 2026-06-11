#include <stdio.h>
#include <string.h>
#include "renderer.h"
#include "gayinvaders.h"

#define MAGENTA_COLOR 61502

static uint16_t *_screen_buffers[SCREEN_FRAMES_X][SCREEN_FRAMES_Y];

void renderer_init(void)
{
	int x, y;

	for (x = 0; x < SCREEN_FRAMES_X; ++x) {
		for (y = 0; y < SCREEN_FRAMES_Y; ++y) {
			_screen_buffers[x][y] = gayinvaders_malloc((2 * (SCREEN_W/SCREEN_FRAMES_X) * (SCREEN_H/SCREEN_FRAMES_Y)));
			if (!_screen_buffers[x][y])
				printf("Failed to allocate screen buffer!\n");
		}
	}
}

void renderer_clear(void)
{
	int x, y;

	for (x = 0; x < SCREEN_FRAMES_X; ++x)
		for (y = 0; y < SCREEN_FRAMES_Y; ++y)
			memset(_screen_buffers[x][y], 0, (2 * (SCREEN_W/SCREEN_FRAMES_X) * (SCREEN_H/SCREEN_FRAMES_Y)));
}

void renderer_render(const render_obj_t *ro)
{
	int start_x, start_y;
	game_object_t *go;
	int end_x, end_y;
	int img_x, img_y;

	go = ro->parent;
	if (!go->active)
		return;

	start_x = go->x - ((float)ro->w / 2);
	start_y = go->y - ((float)ro->h / 2);
	end_x = go->x + ((float)ro->w / 2);
	end_y = go->y + ((float)ro->h / 2);

	img_x = 0;
	img_y = 0;

	for (; start_x < end_x; ++start_x) {
		int y;


		img_y = 0;

		if (start_x < 0)
			continue;

		for (y = start_y; y < end_y; ++y) {
			uint16_t pix;
			int x = start_x;

			if (y < 0) {
				++img_y;
				continue;
			}

			pix = ro->buff[img_y * ro->w + img_x];

			++img_y;

			/* Magenta marks transparency */
			if (pix == MAGENTA_COLOR)
				continue;

			_screen_buffers[x/(SCREEN_W/SCREEN_FRAMES_X)]
				       [y/(SCREEN_H/SCREEN_FRAMES_Y)]
				       [(y%(SCREEN_H/SCREEN_FRAMES_Y))*(SCREEN_W/SCREEN_FRAMES_X)+(x%(SCREEN_W/SCREEN_FRAMES_X))] = pix;

		}
		++img_x;
	}
}

void renderer_flush(void)
{
	/* gayinvaders_render() takes a uint16_t*** (a table of row pointers),
	 * but _screen_buffers is a 2D array (uint16_t*[X][Y]) whose pointers sit
	 * in one contiguous block. Build an actual array of row pointers so the
	 * triple indirection resolves correctly. */
	const uint16_t **rows[SCREEN_FRAMES_X];
	int i;

	for (i = 0; i < SCREEN_FRAMES_X; ++i)
		rows[i] = (const uint16_t **)_screen_buffers[i];

	gayinvaders_render((const uint16_t ***)rows);
}
