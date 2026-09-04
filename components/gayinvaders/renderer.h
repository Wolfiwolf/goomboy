#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdint.h>
#include <stdbool.h>
#include "gameobject.h"
#include "wd.h"

typedef struct {
	game_object_t *parent;
	const uint16_t *buff;
	int w, h;
} render_obj_t;

void renderer_init(void);

void renderer_clear(void);

void renderer_render(const render_obj_t *ro);

void renderer_flush(void);

/* Stream an asset to the display without allocating a full-screen framebuffer. */
void renderer_stream_asset(asset_type_t asset);

void renderer_dont_flush(void);

void renderer_flip_next(void);

#endif
