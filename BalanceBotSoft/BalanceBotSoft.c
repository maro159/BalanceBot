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
#include "acc_gyro.h"
#include "controler.h"

volatile bool time_to_go = false; 
bool is_run = false;
bool was_run = false;
static int addr = 0x68;

uint32_t last_time_us = 0;

const uint32_t sampling_time_us = 20 * 1000;
float sampling_time_sec = sampling_time_us / (1000 * 1000);

repeating_timer_t controler_timer;

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

/*uint32_t convert_pwm(int value, bool *direction)
{
    *direction = DIR_FORWARD;
    if(value < 0) *direction = DIR_REVERSE;

    value = abs(value);
    if (value > 65535) value = 65535;
    return value;
}
*/

bool controler_timer_callback()
{
    // current_time = time_ms();
    time_to_go = true; 
    return true; //to continue repeating 
}

void controler_timer_set()
{
    uint32_t interval_ms = sampling_time_us / 1000;
    add_repeating_timer_ms(interval_ms,&controler_timer_callback, NULL, &controler_timer);
}


int main() 
{
    init();
    init_motors();
    // init_servo();
    init_oled();
    init_encoder();
    init_acc_gyro();
    init_controler();
    init_menu();

    controler_timer_set();
    while(true)
    {
        if(time_to_go)// && current_menu == RUN
        {
            uint32_t current_time_us = time_us_32();
            uint32_t dt_us = current_time_us - last_time_us;
            last_time_us = current_time_us; 
            
            controler_update();
            time_to_go = false;
        }
        
        is_run = menu_execute(); // indicates if we are in run menu
        if(!was_run && is_run) controler_run();
        else if(was_run && !is_run) controler_stop();
        was_run = is_run;
    }
    
}

/*
int main()
{    
    Init();
    init_motors();
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
