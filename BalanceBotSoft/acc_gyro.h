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
#define ACCEL_CONFIG    0x1C
#define USER_CTRL       0x6A
#define MPU6050_ADDR    0x68

extern float acc_angle_deg;
extern float gyro_angular;

void acc_gyro_print();
void init_acc_gyro();
void mpu6050_read_data();
// void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
// void acc_get_angle();
// void gyro_get_angular();
// void gyro_alarm();


#endif