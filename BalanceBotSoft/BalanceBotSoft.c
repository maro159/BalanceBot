#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
#include "oled.h"
#include "encoder_rot.h"
#include "menu.h"
#include "acc_gyro.h"


static int addr = 0x68;

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




menu_t *current_menu;
set_t *current_values;
const int32_t menu_main_order = 0;
const int32_t menu_properties_order = 1;
bool return_to_main_loop = false; 



int main()
{    
    Init();
    mpu6050_reset();
    while(true)
        {
            // acc_gyro_read();
        }     
    // oled_init();
    // current_menu = &menu_main;
    // oled_show_menu(current_menu);
    // oled_x(counter_en);
    // encoder_init(0, current_menu->count);     
    
    
    
    // while(true)
    // {         
    //     if(encoder_changed) 
    //     {
    //         if(current_menu == &menu_main || current_menu == &menu_properties)
    //         {
    //             oled_x(counter_en);
    //         }
    //         else
    //         {
    //             // iNNE MENU
    //         }
    //     }

    //     if(status_SW)
    //     {
    //         status_SW = false;
    //         if(current_menu == &menu_main || current_menu == &menu_properties)
    //         {
    //             current_menu = current_menu->options[counter_en].ptr;
    //             if(current_menu == NULL) {oled_clear(); break;}            
    //             counter_en = 0;
    //             last_count = 0;
    //             oled_clear();
    //             oled_show_menu(current_menu);   
    //         }
    //         else
    //         {
    //             // iNNE MENU
    //         }
    //     }
             
    // }
}
