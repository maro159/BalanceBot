#ifndef _ACC_GYRO_H_
#define _ACC_GYRO_H_

#include <stdio.h>
#include "pico/stdlib.h"



extern int16_t acceleration[];
extern int16_t gyro[];
extern int16_t temp;

void acc_gyro_read();
void mpu6050_reset();
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);


#endif