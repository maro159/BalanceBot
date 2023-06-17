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


volatile bool status_A = false;
volatile bool status_B = false;

void a_handler(){

    status_A = !status_A;   
    printf(" A");
   
    
}
void b_handler(){

    // status_B = !status_B;   
    printf("B");
   
     
}


int main()
{

    Init();

    // stdio_init_all();    
    // stdio_set_uart_enabled(uart0, true);
    // uart_set_baudrate(uart0, 115200);

    uart_puts(BT_UART, "Hello, MPU6050! Reading raw data from registers...\n");
    mpu6050_reset();

    //ENKODER
    gpio_set_irq_enabled_with_callback(ENC_ROT_B,GPIO_IRQ_EDGE_RISE,true,&b_handler);
    gpio_set_irq_enabled_with_callback(ENC_ROT_A,GPIO_IRQ_EDGE_FALL,true,&a_handler);
    

   
    
    while(true){
        tight_loop_contents();      
    
        
    }



/*    //BUZZ TEST
    while(true){
        gpio_put(BUZZ,1);
        sleep_ms(1000);
        gpio_put(BUZZ,0);
        sleep_ms(500);
    }
 */   


/* //odczyt acc,gyro,temp
    int16_t acceleration[3], gyro[3], temp;
while (1) {
        mpu6050_read_raw(acceleration, gyro, &temp);

        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
        printf("Temp. = %f\n", (temp / 340.0) + 36.53);
        printf("dupa bobra");

        sleep_ms(1000);
    }
*/

    
    /*stdio_init_all();

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    

    // GPIO initialisation.
    // We will make this GPIO an input, and pull it up by default
    gpio_init(GPIO);
    gpio_set_dir(GPIO, GPIO_IN);
    gpio_pull_up(GPIO);
    

    // Example of using the HW divider. The pico_divider library provides a more user friendly set of APIs 
    // over the divider (and support for 64 bit divides), and of course by default regular C language integer
    // divisions are redirected thru that library, meaning you can just use C level `/` and `%` operators and
    // gain the benefits of the fast hardware divider.
    int32_t dividend = 123456;
    int32_t divisor = -321;
    // This is the recommended signed fast divider for general use.
    divmod_result_t result = hw_divider_divmod_s32(dividend, divisor);
    printf("%d/%d = %d remainder %d\n", dividend, divisor, to_quotient_s32(result), to_remainder_s32(result));
    // This is the recommended unsigned fast divider for general use.
    int32_t udividend = 123456;
    int32_t udivisor = 321;
    divmod_result_t uresult = hw_divider_divmod_u32(udividend, udivisor);
    printf("%d/%d = %d remainder %d\n", udividend, udivisor, to_quotient_u32(uresult), to_remainder_u32(uresult));

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);

    // Watchdog example code
    if (watchdog_caused_reboot()) {
        // Whatever action you may take if a watchdog caused a reboot
    }
    
    // Enable the watchdog, requiring the watchdog to be updated every 100ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(100, 1);
    
    // You need to call this function at least more often than the 100ms in the enable call to prevent a rebootwatchdog_update();



    puts("Hello, world!");
    */

    return 0;
}
