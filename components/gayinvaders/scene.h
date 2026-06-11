#ifndef SCENE_H_
#define SCENE_H_

typedef struct {
	void (*init)(void);
	void (*update)(float dt);
	void (*end)(void);
	int (*change_scene)(void);
} scene_t;

typedef enum {
	SCENE_TYPE_INTRO,
	SCENE_TYPE_GAME,
	SCENE_TYPE_COUNT,
} scene_type_t;

scene_t *scenes_get_intro_scene(void);
scene_t *scenes_get_game_scene(void);

#endif
