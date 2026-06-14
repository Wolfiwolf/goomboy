#include <stdio.h>
#include <string.h>
#include "renderer.h"
#include "gayinvaders.h"

#define MAGENTA_COLOR 61502

static uint16_t *_screen_buffers[SCREEN_FRAMES_X][SCREEN_FRAMES_Y];
static bool _dont_flush = false;
static bool _flip_next = false;
static bool _buffer_lock = false;

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

	while (_buffer_lock) { }

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

	while (_buffer_lock) { }

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

		if (start_x < 0) {
			img_x++;
			continue;
		}

		if (start_x >= SCREEN_W)
			break;

		for (y = start_y; y < end_y; ++y) {
			uint16_t pix;
			int x = start_x;

			if (y < 0) {
				++img_y;
				continue;
			}

			if (y >= SCREEN_H)
				break;

			if (_flip_next)
				pix = ro->buff[img_y * ro->w + (ro->w-img_x)];
			else
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

	_flip_next = false;
}

void renderer_flush(void)
{
	const uint16_t **rows[SCREEN_FRAMES_X];
	int i;

	if (_dont_flush) {
		_dont_flush = false;
		return;
	}

	while (_buffer_lock) { }

	for (i = 0; i < SCREEN_FRAMES_X; ++i)
		rows[i] = (const uint16_t **)_screen_buffers[i];

	_buffer_lock = true;
	gayinvaders_render((const uint16_t ***)rows);
}

void renderer_dont_flush(void)
{
	_dont_flush = true;
}

void renderer_buffer_unlock(void)
{
	_buffer_lock = false;
}

void renderer_flip_next(void)
{
	_flip_next = true;
}
