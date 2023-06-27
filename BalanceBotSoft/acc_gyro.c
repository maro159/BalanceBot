#include "acc_gyro.h"
#include "pins.h"
#include "hardware/i2c.h"



float acc_angle_deg;
float gyro_angular;



// uint32_t current_time;
// float previous_time;
// float loop_time;



void init_acc_gyro()
{
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {INT_ENABLE_REG, 1};
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf, 2, false);
    uint8_t buf2[] = {SMPLRT_DIV_REG, 19}; // sampling frequency 
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf2, 2, false);
    uint8_t buf3[] = {PWR_MGMT_1_REG, 1};
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf3, 2, false);
    uint8_t buf4[] = {CONFIG_REG, 0};
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf4, 2, false);
    uint8_t buf5[] = {GYRO_CONFIG, 0};
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf5, 2, false);
    uint8_t buf6[] = {ACCEL_CONFIG, 0};
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf6, 2, false);
    //reset FIFO 
    uint8_t buf7[] = {USER_CTRL,0x44}; 
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf7, 2, false);
    sleep_ms(20); //można zmienić jak będzie za dużo/za mało, fifo usuwa dane w kilka ms 
    uint8_t buf8[] = {USER_CTRL,0x40}; 
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, buf8, 2, false);

    //TODO sprawdzić i ustawić offsety 

}

void mpu6050_read_data()
{
    uint8_t buffer[6];
    int16_t accel[3];
    int16_t gyro[3];

     // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(IMU_I2C, MPU6050_ADDR, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, &val, 1, true);
    i2c_read_blocking(IMU_I2C, MPU6050_ADDR, buffer, 6, false); // False - finished with bus

    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
        
    }
    
    int16_t acc_x = accel[0];
    int16_t acc_y = accel[1];
    int16_t acc_z = accel[2];

    acc_angle_deg = atan2(acc_z, -acc_x) * (180/M_PI);

    int16_t gyro_x = gyro[0];
    int16_t gyro_y = gyro[1];
    int16_t gyro_z = gyro[2];

    gyro_angular = gyro_y / 131;
    
   
}




void gyro_calc_angular()
{

}



/*
// void acc_gyro_print()
// {
//     mpu6050_read_raw(acceleration, gyro, &temp);

//     // These are the raw numbers from the chip, so will need tweaking to be really useful.
//     // See the datasheet for more information
//     printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
//     printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
//     // Temperature is simple so use the datasheet calculation to get deg C.
//     // Note this is chip temperature.
//     printf("Temp. = %f\n", (temp / 340.0) + 36.53);
//     //sleep_ms(1000); //TODO dlatego printował z mniejsza częstotliwością 
//     printf("Angle [deg]: ", acc_angle_deg);

// }

/*void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) 
{
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(IMU_I2C, MPU6050_ADDR, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, &val, 1, true);
    i2c_read_blocking(IMU_I2C, MPU6050_ADDR, buffer, 6, false); // False - finished with bus

    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
        ;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(IMU_I2C, MPU6050_ADDR, &val, 1, true);
    i2c_read_blocking(IMU_I2C, MPU6050_ADDR, buffer, 2, false); // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];

}
*/



