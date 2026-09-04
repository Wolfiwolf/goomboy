#include <stdio.h>
#include <string.h>
#include "renderer.h"
#include "gayinvaders.h"

#define MAGENTA_COLOR 61502
#define RENDER_STRIP_H 20
#define RENDER_MAX_COMMANDS 192

typedef struct {
	const uint16_t *pixels;
	int x, y;
	int w, h;
	bool flipped;
} render_command_t;

static uint16_t *_strip;
static render_command_t _commands[RENDER_MAX_COMMANDS];
static size_t _commands_count;
static bool _dont_flush;
static bool _flip_next;
static bool _overflow_logged;

static void composite(const render_command_t *command, int strip_y, int strip_h)
{
	int first_x = command->x < 0 ? 0 : command->x;
	int last_x = command->x + command->w;
	int first_y = command->y > strip_y ? command->y : strip_y;
	int last_y = command->y + command->h;
	int x, y;

	if (last_x > SCREEN_W)
		last_x = SCREEN_W;
	if (last_y > strip_y + strip_h)
		last_y = strip_y + strip_h;
	if (first_x >= last_x || first_y >= last_y)
		return;

	for (y = first_y; y < last_y; ++y) {
		int src_y = y - command->y;
		int dst_y = y - strip_y;

		for (x = first_x; x < last_x; ++x) {
			int src_x = x - command->x;
			uint16_t pix;

			if (command->flipped)
				src_x = command->w - 1 - src_x;
			pix = command->pixels[src_y * command->w + src_x];
			if (pix != MAGENTA_COLOR)
				_strip[dst_y * SCREEN_W + x] = pix;
		}
	}
}

static int present_strip(int strip_y, int strip_h)
{
	memset(_strip, 0, SCREEN_W * strip_h * sizeof(*_strip));

	for (size_t i = 0; i < _commands_count; ++i)
		composite(&_commands[i], strip_y, strip_h);

	return gayinvaders_present(_strip, 0, strip_y, SCREEN_W, strip_h);
}

void renderer_init(void)
{
	_strip = gayinvaders_malloc(SCREEN_W * RENDER_STRIP_H * sizeof(*_strip));
	if (!_strip)
		printf("Failed to allocate LCD strip buffer!\n");
}

void renderer_clear(void)
{
	_commands_count = 0;
	_overflow_logged = false;
}

void renderer_render(const render_obj_t *ro)
{
	game_object_t *go;
	render_command_t *command;

	if (!ro || !ro->parent || !ro->buff || ro->w <= 0 || ro->h <= 0)
		return;

	go = ro->parent;
	if (!go->active)
		return;

	if (_commands_count == RENDER_MAX_COMMANDS) {
		if (!_overflow_logged) {
			printf("Renderer command limit (%d) reached\n", RENDER_MAX_COMMANDS);
			_overflow_logged = true;
		}
		_flip_next = false;
		return;
	}

	command = &_commands[_commands_count++];
	command->pixels = ro->buff;
	command->x = go->x - ((float)ro->w / 2);
	command->y = go->y - ((float)ro->h / 2);
	command->w = ro->w;
	command->h = ro->h;
	command->flipped = _flip_next;
	_flip_next = false;
}

void renderer_flush(void)
{
	int y;

	if (_dont_flush) {
		_dont_flush = false;
		return;
	}
	if (!_strip) {
		printf("Cannot flush without an LCD strip buffer\n");
		return;
	}

	for (y = 0; y < SCREEN_H; y += RENDER_STRIP_H) {
		int strip_h = SCREEN_H - y;

		if (strip_h > RENDER_STRIP_H)
			strip_h = RENDER_STRIP_H;
		if (present_strip(y, strip_h))
			break;
	}
}

void renderer_stream_asset(asset_type_t asset)
{
	const asset_info_t *info = wd_get_asset_info(asset);
	uint16_t *source;
	int y;

	if (!_strip || !info || info->w <= 0 || info->h <= 0)
		return;

	source = gayinvaders_malloc(info->w * RENDER_STRIP_H * sizeof(*source));
	if (!source) {
		printf("Failed to allocate asset strip buffer\n");
		return;
	}

	for (y = 0; y < SCREEN_H; y += RENDER_STRIP_H) {
		render_command_t command = {
			.pixels = source,
			.x = 0,
			.y = y,
			.w = info->w,
			.h = 0,
		};
		int strip_h = SCREEN_H - y;

		if (strip_h > RENDER_STRIP_H)
			strip_h = RENDER_STRIP_H;
		memset(_strip, 0, SCREEN_W * strip_h * sizeof(*_strip));

		if (y < info->h) {
			command.h = info->h - y;
			if (command.h > strip_h)
				command.h = strip_h;
			if (wd_read_asset_direct(asset, source, 0, y, info->w, command.h)) {
				printf("Failed to read streamed asset\n");
				break;
			}
			composite(&command, y, strip_h);
		}

		if (gayinvaders_present(_strip, 0, y, SCREEN_W, strip_h))
			break;
	}

	gayinvaders_free(source);
}

void renderer_dont_flush(void)
{
	_dont_flush = true;
}

void renderer_flip_next(void)
{
	_flip_next = true;
}
