#ifndef _MENU_H_
#define _MENU_H_
#include "pico/stdlib.h"

#define MENU_SIZE(m) (sizeof(m)/sizeof(m[0]))

typedef enum menu_type_t
{
    MENU_NORMAL,
    MENU_PARAM,
} menu_type_t;

typedef struct option_t
{
    char* name;
    void* ptr;
} option_t;

typedef struct limit_t
{
    float min;
    float max;
    float step;
    
} limit_t;

typedef struct menu_t
{
    menu_type_t menu_type;
    option_t *options;  // pointer to array
    limit_t limits;
} menu_t;

extern menu_t menu_main;
extern menu_t menu_settings;
extern menu_t menu_pids;
extern menu_t menu_pid_speed;
extern menu_t menu_pid_imu;
extern menu_t menu_pid_motor;

extern menu_t menu_pid_speed_kp;
extern menu_t menu_pid_speed_ki;
extern menu_t menu_pid_speed_kd;

extern menu_t menu_pid_imu_kp;
extern menu_t menu_pid_imu_ki;
extern menu_t menu_pid_imu_kd;

extern menu_t menu_pid_motor_kp;
extern menu_t menu_pid_motor_ki;
extern menu_t menu_pid_motor_kd;

extern menu_t menu_motor_power;
extern menu_t menu_max_angle;

void menu_init();
void menu_bind_parameter(void *param, menu_t *menu);
int32_t menu_execute();

#endif
