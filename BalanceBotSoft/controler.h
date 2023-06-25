#ifndef _CONTROLER_H_
#define _CONTROLER_H_

#include "pid.h"

extern pid_ctrl pid_speed;
extern pid_ctrl pid_imu;
extern pid_ctrl pid_motor_a;
extern pid_ctrl pid_motor_b;

void controler_init();
void controler_update();
#endif