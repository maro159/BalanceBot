#ifndef _IMU_H_
#define _IMU_H_

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

void init_imu();
void imu_get_data(float *acc_angle_deg, float *gyro_angular);
// void imu_print();
// void imu_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
#endif