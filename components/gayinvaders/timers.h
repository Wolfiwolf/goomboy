#ifndef TIMERS_H_
#define TIMERS_H_

void timers_init(void);

void timers_update(float dt);

void timers_start(int duration, void *data, void (*on_finish)(void *data));

#endif
