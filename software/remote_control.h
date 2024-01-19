#ifndef _REMOTE_CONTROL_H_
#define _REMOTE_CONTROL_H_
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define def_check_mark 'f'
#define def_right_turn
#define def_left_turn
#define def_no_turn


typedef struct remote_targets_t
{
    float robot_speed;
    float turn_speed;
} remote_targets_t; 

extern recursive_mutex_t remote_control_mutex;
void remote_control_run();
bool try_get_remote_target_speed(float *out);
bool try_get_remote_data(struct remote_targets_t *data_frame);
#endif
