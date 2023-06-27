#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "motor.h"
#include "pins.h"
#include "quadrature_encoder.pio.h"

#include "hardware/pio.h"
#include "hardware/pwm.h"

float motor_power_ratio = 1;

void _set_pwm(Motor motor, uint16_t pwm, bool direction)
{
    if(motor == MOTOR_A)    // motor A
    {
        #if MOTOR_A_INVERT == 1 // TODO: readability over performance
        direction = !direction; 
        #endif
        if (direction == DIR_FORWARD)
        {
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A1_CHAN, pwm);
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A2_CHAN, 0);
        }
        else
        {
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A1_CHAN, 0);
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A2_CHAN, pwm);
        }
    }
    else    // motor B
    {
        #if MOTOR_B_INVERT == 1  // TODO: readability over performance
        direction = !direction;
        #endif
        if (direction == DIR_FORWARD)
        {
            pwm_set_chan_level(MOTOR_B_SLICE, MOTOR_B1_CHAN, pwm);
            pwm_set_chan_level(MOTOR_B_SLICE, MOTOR_B2_CHAN, 0);
        }
        else
        {
            pwm_set_chan_level(MOTOR_B_SLICE, MOTOR_B1_CHAN, 0);
            pwm_set_chan_level(MOTOR_B_SLICE, MOTOR_B2_CHAN, pwm);
        }
    }
}

void init_motors()
{
    // // ** motor driver outputs **
    // gpio_set_function(MOTOR_A1, GPIO_FUNC_PWM);
    // gpio_set_function(MOTOR_A2, GPIO_FUNC_PWM);
    // gpio_set_function(MOTOR_B1, GPIO_FUNC_PWM);
    // gpio_set_function(MOTOR_B2, GPIO_FUNC_PWM);

    // ** pwm settings for motor driver **
    _set_pwm(MOTOR_A, 0, DIR_FORWARD);
    _set_pwm(MOTOR_B, 0, DIR_FORWARD);
    pwm_set_enabled(MOTOR_A_SLICE, true);
    pwm_set_enabled(MOTOR_B_SLICE, true);

    // ** motor encoders inputs **
    uint offset_pio = pio_add_program(MOTOR_ENC_PIO, &quadrature_encoder_program);
    quadrature_encoder_program_init(MOTOR_ENC_PIO, MOTOR_A, offset_pio, MOTOR_ENC_AA, ENC_MAX_STEPRATE);  // state machine 0
    quadrature_encoder_program_init(MOTOR_ENC_PIO, MOTOR_B, offset_pio, MOTOR_ENC_BA, ENC_MAX_STEPRATE);  // state machine 1
}

void motor_encoder_request()
{
    quadrature_encoder_request_count(MOTOR_ENC_PIO, MOTOR_A); // motor A
    quadrature_encoder_request_count(MOTOR_ENC_PIO, MOTOR_B); // motor B
}

int32_t motor_encoder_get(Motor motor)
{
    return quadrature_encoder_fetch_count(MOTOR_ENC_PIO, motor);    // TODO: divide by 4 or not
}

void motor_limit(float motor_ratio)
{
    if(motor_ratio > 1) motor_ratio = 1; 
    motor_power_ratio = motor_ratio; 
}

void motor_set_power(Motor motor, float power)
{
    bool dir; 
    if(power >= 0) dir = DIR_FORWARD;
    else dir = DIR_REVERSE; 

    power = fabsf(power); 
    if (power > 1) power = 1;
    uint16_t pwm = (uint16_t)(power * motor_power_ratio * 32767); 
   
    _set_pwm(motor, pwm, dir); 

    #ifdef DEBUG_MODE
    // printf("%d\t%d\n",pwm,dir);
    #endif

}