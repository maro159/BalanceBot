#ifndef _CONTROLER_H_
#define _CONTROLER_H_

#include "pid.h"

extern float zero_angle;
extern pid_ctrl pid_speed;
extern pid_ctrl pid_imu;
extern pid_ctrl pid_motor_a;
extern pid_ctrl pid_motor_b;

void init_controler();
void controler_update();
void controler_stop();
void controler_run();
#endif