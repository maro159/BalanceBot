#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/divider.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pins.h"
#include "motor.h"
#include "oled.h"
#include "encoder_rot.h"
#include "menu.h"
#include "imu.h"
#include "controler.h"

static volatile bool time_to_go = false; 
const uint32_t sampling_time_us = 10 * 1000;
uint32_t last_time_us = 0;

static void init()
{
    i2c_init(IMU_I2C, IMU_BAUDRATE);  
    gpio_set_function(IMU_SDA, GPIO_FUNC_I2C);
    gpio_set_function(IMU_SCL, GPIO_FUNC_I2C);
    gpio_init(IMU_INT);
    gpio_set_dir(IMU_INT, false);

    i2c_init(OLED_I2C, OLED_BAUDRATE);
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);
    
    gpio_set_function(MOTOR_A1, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_A2, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_B1, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_B2, GPIO_FUNC_PWM);

    gpio_set_function(SERWO, GPIO_FUNC_PWM);

    // uart_init(BT_UART, BT_BAUDRATE);
    stdio_uart_init_full(BT_UART, BT_BAUDRATE, BT_TX, BT_RX);
    // stdio_init_all();
    gpio_set_function(BT_TX, GPIO_FUNC_UART);
    gpio_set_function(BT_RX, GPIO_FUNC_UART);
    
    adc_init();
    adc_gpio_init(BATTERY_LVL);
    adc_select_input(BATTERY_LVL_ADC);
        
    gpio_init(HC04_A_TRIG);
    gpio_init(HC04_B_TRIG); 
    gpio_set_dir(HC04_A_TRIG, true);
    gpio_set_dir(HC04_B_TRIG, true);

    gpio_init(HC04_A_ECHO);
    gpio_init(HC04_B_ECHO); 
    gpio_set_dir(HC04_A_ECHO, false);
    gpio_set_dir(HC04_B_ECHO, false);

    gpio_init(BUZZ); 
    gpio_set_dir(BUZZ, true);

    gpio_init(NEOPX); 
    gpio_set_dir(NEOPX, true);

    gpio_init(ENC_ROT_A);
    gpio_init(ENC_ROT_B);
    gpio_init(ENC_ROT_SW); 
    gpio_set_dir(ENC_ROT_A, false);
    gpio_set_dir(ENC_ROT_B, false);
    gpio_set_dir(ENC_ROT_SW, false);
}

bool controler_timer_callback(repeating_timer_t *t)
{
    time_to_go = true; 
    return true; //to continue repeating 
}

int main() 
{
    bool is_run = false;
    bool was_run = false;

    repeating_timer_t controler_timer;
    init();
    init_motor();
    // init_servo();
    init_oled();
    init_encoder();
    init_imu();
    init_controler(sampling_time_us);
    init_menu();
    /* Negative delay so means we will call repeating_timer_callback, and call it again 
     * regardless of how long the callback took to execute */
    add_repeating_timer_us(-(int64_t)(sampling_time_us), &controler_timer_callback, NULL, &controler_timer); 
    while(true)
    {
        if(time_to_go)
        {
            uint32_t current_time_us = time_us_32();
            uint32_t dt_us = current_time_us - last_time_us;
            last_time_us = current_time_us;
            // printf("*%u\n*",dt_us);
            controler_update();
            // printf("*%u\n*",time_us_32()-last_time_us);
            printf("%f,%f,%f,%f,%f,%f\n",*pid_speed->setpoint, *pid_speed->input, *pid_imu->setpoint, *pid_imu->input, *pid_motor_a->setpoint, *pid_motor_a->input);
            
            time_to_go = false;
        }
        if(menu_get() == &menu_run) is_run = true;
        else is_run = false;

        if(!was_run && is_run) controler_run();
        else if(was_run && !is_run) controler_stop();
        was_run = is_run;
    }
}

/*
int main()
{    
    Init();
    init_motor();
    const uint32_t timeout_us = 1000;
    uint8_t buf[30];
    uint32_t buf_head = 0;
    Motor motor = MOTOR_A;

    while(true)
    {
        int c = getchar_timeout_us(timeout_us);	
        if (c != PICO_ERROR_TIMEOUT)
        {
            buf[buf_head++] = c;
            if (c == 0x0D)
            {
                
                // Convert the uint8_t array to a string
                char str[buf_head];
                strncpy(str, (char*)buf, buf_head);
                str[buf_head] = '\0';
                printf(str);
                // Convert the string to an integer
                int value = atoi(str);
                printf("Converted: %d", value);
                bool direction = DIR_FORWARD;
                uint32_t pwm = convert_pwm(value, &direction);
                printf("Value: %d, direction: %d", pwm, direction);
                // motor_set_speed(motor, pwm, direction);
                buf_head = 0;
            }
            else if (c == 'A')
            {
                motor = MOTOR_A;
                printf("Motor A\n");
                buf_head=0;
            }
            else if (c == 'B')
            {
                motor = MOTOR_B;
                printf("Motor B\n");
                buf_head=0;
            }
        }
        
    }
    

    return 0;
}*/
