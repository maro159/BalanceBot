#ifndef _ACC_GYRO_H_
#define _ACC_GYRO_H_

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"

#define PWR_MGMT_1_REG  0x6B
#define SMPLRT_DIV_REG  0x19
#define CONFIG_REG      0x1A
#define INT_ENABLE_REG  0x38
#define GYRO_CONFIG     0x1B
#define USER_CTRL       0X6A

extern int16_t acceleration[];
extern int16_t gyro[];
extern int16_t temp;

extern int16_t acc_y;
extern int16_t acc_z;

extern float acc_angle_rad;
extern float acc_angle_deg;

extern float gyro_angle;
extern float gyro_rate;

extern float current_time;
extern float previous_time;
extern float loop_time;



void acc_gyro_read();
void mpu6050_init();
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
void acc_get_angle();
void gyro_get_angle();
void gyro_alarm();
float gyro_map(float gyro_x, float in_min, float in_max, float out_min, float out_max);



#endif