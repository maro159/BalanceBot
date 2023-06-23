#ifndef _MOTOR_H_
#define _MOTOR_H_


#define MOTOR_A_INVERT 0
#define MOTOR_B_INVERT 0
#define ENC_MAX_STEPRATE 0

#define MOTOR_ENC_PIO pio0
#define DIR_FORWARD true
#define DIR_REVERSE false

typedef enum Motor
{
    MOTOR_A = 0,
    MOTOR_B = 1,
} Motor;

void init_motors();
void init_servo();
void set_servo_position();
void motor_set_speed(Motor motor, uint16_t speed, bool direction);
void motor_encoder_request();
uint32_t motor_encoder_get(Motor motor);


#endif
