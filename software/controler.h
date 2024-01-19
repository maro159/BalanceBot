#ifndef _CONTROLER_H_
#define _CONTROLER_H_

#include "pid.h"
#include "filters.h"

#define REMOTE_SPEED_FACTOR 15.0f
#define REMOTE_TURN_FACTOR 4.0f

extern float zero_angle;
extern pid_ctrl pid_speed;
extern pid_ctrl pid_imu;
extern pid_ctrl pid_motor_a;
extern pid_ctrl pid_motor_b;
extern IIR_filter_t iir;

void init_controler(uint32_t sampling_time_us);
void controler_update();
void controler_stop();
void controler_run();
#endif