#ifndef _REMOTE_CONTROL_H_
#define _REMOTE_CONTROL_H_
#include "pico/stdlib.h"
#include "pico/multicore.h"

extern recursive_mutex_t remote_control_mutex;
void remote_control_run();
bool try_get_remote_target_speed(float *out);
#endif