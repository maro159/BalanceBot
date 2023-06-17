#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define IMU_SDA 0
#define IMU_SCL 1
#define IMU_INT 6
#define IMU_I2C i2c0
#define IMU_BAUDRATE 400*1000

#define OLED_SDA 14
#define OLED_SCL 15
#define OLED_I2C i2c1
#define OLED_BAUDRATE 400*1000

#define BT_TX 12
#define BT_RX 13
#define BT_UART uart0
#define BT_BAUDRATE 9600

#define SERWO 11
// #define SERWO_CHANNEL pwm_gpio_to_channel 
// #define SERWO_SLICE 5

#define MOTOR_A1 3
#define MOTOR_A2 2
#define MOTOR_B1 5
#define MOTOR_B2 4
// #define MOTOR_A_SLICE 1 
// #define MOTOR_B_SLICE 2

#define MOTOR_ENC_AA 7
#define MOTOR_ENC_AB 8
#define MOTOR_ENC_BA 9
#define MOTOR_ENC_BB 10

#define ENC_ROT_A 17
#define ENC_ROT_B 18
#define ENC_ROT_SW 16

#define HC04_A_TRIG 20
#define HC04_B_TRIG 26
#define HC04_A_ECHO 19
#define HC04_B_ECHO 21

#define BUZZ 22
#define NEOPX 27 // albo EEPROM_WP

#define BATTERY_LVL 28
#define BATTERY_LVL_ADC 2 // adc channel 


static void Init()
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
       
    uart_init(BT_UART, BT_BAUDRATE);
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


