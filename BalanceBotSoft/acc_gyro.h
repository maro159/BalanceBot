#ifndef _ACC_GYRO_H_
#define _ACC_GYRO_H_

#include <stdio.h>
#include "pico/stdlib.h"

extern int16_t acceleration[];
extern int16_t gyro[];
extern int16_t  temp;

void acc_gyro_read();

#endif