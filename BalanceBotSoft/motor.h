#ifndef _MOTOR_H_
#define _MOTOR_H_

// change if direction of motor is wrong
#define MOTOR_A_INVERT 0
#define MOTOR_B_INVERT 1

// change if
#define ENC_A_INVERT 0
#define ENC_B_INVERT 1

#define ENC_MAX_STEPRATE 0

#define MOTOR_ENC_PIO pio0
#define DIR_FORWARD true
#define DIR_REVERSE false

#define MOTOR_ENCODER_TICKS 960 // encoder ticks for 1 rotate

typedef enum Motor
{
    MOTOR_A = 0,
    MOTOR_B = 1,
} Motor;

extern float motor_power_ratio;

void init_motor();
void init_servo();
void servo_set_position();
// power should be in range [-1, 1]
void motor_set_power(Motor motor, float power);
// limit motor power. motor_ratio should be in range [-1, 1]
void motor_limit(float motor_ratio);
// latch encoder count
void motor_encoder_request();
 // return latched encoder count, must be preceded with motor_encoder_request();
int32_t motor_encoder_get(Motor motor);


#endif
