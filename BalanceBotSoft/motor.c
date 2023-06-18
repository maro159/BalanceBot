#include <stdio.h>
#include "pico/stdlib.h"

#include "motor.h"
#include "pins.h"
#include "quadrature_encoder.pio.h"

#include "hardware/pio.h"
#include "hardware/pwm.h"

void init_motors()
{
    // ** motor driver outputs **
    gpio_set_function(MOTOR_A1, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_A2, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_B1, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_B2, GPIO_FUNC_PWM);

    // ** pwm settings for motor driver **
    set_motor_speed(MOTOR_A, 0, DIR_FORWARD);
    set_motor_speed(MOTOR_B, 0, DIR_FORWARD);
    pwm_set_enabled(MOTOR_A_SLICE, true);
    pwm_set_enabled(MOTOR_B_SLICE, true);

    // ** motor encoders inputs **
    uint offset_pio = pio_add_program(MOTOR_ENC_PIO, &quadrature_encoder_program);
    quadrature_encoder_program_init(MOTOR_ENC_PIO, MOTOR_A, offset_pio, MOTOR_ENC_AA, ENC_MAX_STEPRATE);  // state machine 0
    quadrature_encoder_program_init(MOTOR_ENC_PIO, MOTOR_B, offset_pio, MOTOR_ENC_BA, ENC_MAX_STEPRATE);  // state machine 1
}

void set_motor_speed(Motor motor, uint16_t speed, bool direction)
{
    if(motor == MOTOR_A)    // motor A
    {
        #if MOTOR_A_INVERT == 1 // TODO: readability over performance
        direction = !direction; 
        #endif
        if (direction == DIR_FORWARD)
        {
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A1_CHAN, speed);
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A2_CHAN, 0);
        }
        else
        {
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A1_CHAN, 0);
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A2_CHAN, speed);
        }
    }
    else    // motor B
    {
        #if MOTOR_B_INVERT == 1  // TODO: readability over performance
        direction = !direction;
        #endif
        if (direction == DIR_FORWARD)
        {
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A1_CHAN, speed);
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A2_CHAN, 0);
        }
        else
        {
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A1_CHAN, 0);
            pwm_set_chan_level(MOTOR_A_SLICE, MOTOR_A2_CHAN, speed);
        }
    }
}

void request_motor_encoders()
{
    quadrature_encoder_request_count(MOTOR_ENC_PIO, MOTOR_A); // motor A
    quadrature_encoder_request_count(MOTOR_ENC_PIO, MOTOR_B); // motor B
}

uint32_t get_motor_encoder(Motor motor)
{
    return quadrature_encoder_fetch_count(MOTOR_ENC_PIO, motor);
}