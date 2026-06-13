#ifndef TIMERS_H_
#define TIMERS_H_

typedef void timer_handle_t;

void timers_init(void);

void timers_update(float dt);

timer_handle_t *timers_start(int duration, bool loop, void *data, void (*on_finish)(void *data));

void timers_stop(timer_handle_t *tim_handle);

#endif
