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

static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(IMU_I2C, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(IMU_I2C, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(IMU_I2C, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(IMU_I2C, addr, &val, 1, true);
    i2c_read_blocking(IMU_I2C, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(IMU_I2C, addr, &val, 1, true);
    i2c_read_blocking(IMU_I2C, addr, buffer, 2, false);  // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
  
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
    oled_init();
    current_menu = &menu_main;
    oled_show_menu(current_menu);
    oled_x(counter_en);
    encoder_init(current_menu->limits->min, current_menu->limits->max);
    
    current_values = &test_values;
    int value = set_value(current_values);
    
    while(true)
    {         
        if(encoder_changed()) 
        {
            printf("%d/%d", last_count, counter_en, last_count);
            if(current_menu->menu_type == MENU_NORMAL)
            {
                oled_x(counter_en);
            }
            else if (current_menu->menu_type == MENU_OPTION )
            {   
                // counter_en = 0;
                int value = set_value(current_values);
                oled_show_values(current_values);
            }
        }

        if(status_SW)
        {
            
            status_SW = false;
            if(current_menu->menu_type == MENU_NORMAL)
            {
                current_menu = current_menu->options[counter_en].ptr;
                encoder_set_limit(current_menu->limits->min, current_menu->limits->max);
                if(current_menu == NULL) {oled_clear(); break;}            
                counter_en = 0;
                last_count = 0;
                oled_clear();
                oled_show_menu(current_menu);   
            }
            else if( current_menu->menu_type == MENU_OPTION)
            {
                current_menu = current_menu->options[1].ptr;
                if(status_SW) 
                {
                    status_SW = false; 
                    current_menu = 
                    break;

                }

            }
        }
             
    }
}
