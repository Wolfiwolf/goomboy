#ifndef SCENE_H_
#define SCENE_H_

typedef struct {
	void (*init)(void);
	void (*update)(float dt);
	void (*render)(void);
	void (*end)(void);
	int (*change_scene)(void);
} scene_t;

typedef enum {
	SCENE_TYPE_INTRO,
	SCENE_TYPE_MAINMENU,
	SCENE_TYPE_BADBOYS,
	SCENE_TYPE_BADBOYSEASY,
	SCENE_TYPE_BADBOYSMOVING,
	SCENE_TYPE_BADBOYSFAST,
	SCENE_TYPE_BADBOYSTANK,
	SCENE_TYPE_BADBOYSBOSS,
	SCENE_TYPE_GAME,
	SCENE_TYPE_DEAD,
	SCENE_TYPE_COUNT,
} scene_type_t;

scene_t *scenes_get_intro_scene(void);
scene_t *scenes_get_mainmenu_scene(void);
scene_t *scenes_get_badboys_scene(void);
scene_t *scenes_get_badboyseasy_scene(void);
scene_t *scenes_get_badboysmoving_scene(void);
scene_t *scenes_get_badboysfast_scene(void);
scene_t *scenes_get_badboystank_scene(void);
scene_t *scenes_get_badboysboss_scene(void);
scene_t *scenes_get_dead_scene(void);
scene_t *scenes_get_game_scene(void);
scene_t *scenes_get_dead_scene(void);

#endif
