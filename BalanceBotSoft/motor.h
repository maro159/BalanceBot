#ifndef _MOTOR_H_
#define _MOTOR_H_


#define MOTOR_A_INVERT 0
#define MOTOR_B_INVERT 0
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

void init_motors();
void init_servo();
void servo_set_position();
// void motor_set_speed(Motor motor, uint16_t speed, bool direction);
void motor_set_power(Motor motor, float power);
void motor_limit(uint32_t motor_ratio);
void motor_encoder_request();
int32_t motor_encoder_get(Motor motor);


#endif
