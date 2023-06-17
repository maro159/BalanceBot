#ifndef _PINS_H_
#define _PINS_H_

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


static void Init();

#endif