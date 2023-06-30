#include "pico/stdlib.h"
#include "controler.h"
#include "pins.h"
#include "acc_gyro.h"
#include "pid.h"
#include "motor.h"

float last_speeds[100];
uint32_t speed_head = 0;
float mean_speed = 0.0;

float current_robot_speed = 0.0;
float target_robot_speed = 1.0; // to remote control

float zero_angle = 12;
float target_angle = 0.0;
float target_angle_offset = 0.0;

float current_angle = 0.0;
float target_motors_speed = 0.0;   

float current_motor_a_speed = 0.0;
float motor_a_power = 0.0;                

float current_motor_b_speed = 0.0;
float motor_b_power = 0.0;   

struct pid_controller pid_speed_ctrl;
struct pid_controller pid_imu_ctrl;
struct pid_controller pid_motor_a_ctrl;
struct pid_controller pid_motor_b_ctrl;

pid_ctrl pid_speed;
pid_ctrl pid_imu;
pid_ctrl pid_motor_a;
pid_ctrl pid_motor_b;

extern const uint32_t sampling_time_us; 
extern float sampling_time_sec;
int32_t motor_a_enc_last = 0; 
int32_t motor_b_enc_last = 0; 


/*
            pid_speed                   pid_imu                 pid_motor_a                 pid_motor_b
in          &current_robot_speed        &current_angle          &current_motor_a_speed      &current_motor_b_speed
out         &target_angle               &target_motor_speed     &motor_a_power              &motor_a_power
set         &target_robotspeed          &target_angle           &target_motor_speed         &target_motor_speed

*/

void init_controler()
{   
    float kp_speed = 0.5;
    float ki_speed = 0.0;
    float kd_speed = 0.0;

    float kp_imu = 4.0;
    float ki_imu = 0.0;
    float kd_imu = 0.04;

    float kp_motor_a = 0.2;
    float ki_motor_a = 0.0;
    float kd_motor_a = 0.0;
    
    float kp_motor_b = 0.2;
    float ki_motor_b = 0.0;
    float kd_motor_b = 0.0;

    pid_speed = pid_create(&pid_speed_ctrl, &mean_speed, &target_angle_offset, &target_robot_speed, kp_speed, ki_speed, kd_speed); 
    pid_imu = pid_create(&pid_imu_ctrl, &current_angle, &target_motors_speed, &target_angle, kp_imu, ki_imu, kd_imu);
    pid_motor_a = pid_create(&pid_motor_a_ctrl, &current_motor_a_speed, &motor_a_power, &target_motors_speed, kp_motor_a, ki_motor_a, kd_motor_a);
    pid_motor_b = pid_create(&pid_motor_b_ctrl, &current_motor_b_speed, &motor_b_power, &target_motors_speed, kp_motor_b, ki_motor_b, kd_motor_b);

    pid_sample(pid_speed, sampling_time_us);
    pid_sample(pid_imu, sampling_time_us);
    pid_sample(pid_motor_a, sampling_time_us);
    pid_sample(pid_motor_b, sampling_time_us);

    pid_limits(pid_speed, -60, 60); //miny i maxy do wpisania ręcznie
    pid_limits(pid_imu, -10, 10);
    pid_limits(pid_motor_a, -1, 1);
    pid_limits(pid_motor_b, -1, 1);

    // pid_direction(pid_speed, E_PID_REVERSE);

    mpu6050_read_data();
    current_angle = acc_angle_deg;

}

void refresh_data() 
{
    float alpha = 0.994;

    motor_encoder_request();
    mpu6050_read_data();
    
    int32_t motor_a_enc = motor_encoder_get(MOTOR_A);
    int32_t motor_b_enc = motor_encoder_get(MOTOR_B);

    current_motor_a_speed = (motor_a_enc - motor_a_enc_last) / (MOTOR_ENCODER_TICKS * sampling_time_sec);
    current_motor_b_speed = (motor_b_enc - motor_b_enc_last) / (MOTOR_ENCODER_TICKS * sampling_time_sec);
    #if MOTOR_A_INVERT==1
    current_motor_a_speed = -current_motor_a_speed;
    #endif
    #if MOTOR_B_INVERT==1
    current_motor_b_speed = -current_motor_b_speed;
    #endif

    current_angle = alpha * (current_angle + sampling_time_sec * gyro_angular) + (1 - alpha) * acc_angle_deg;
    // current_angle = (current_angle + sampling_time_sec * gyro_angular);    
    // current_angle = alpha * current_angle + (1 - alpha) * acc_angle_deg;
    current_robot_speed = (current_motor_a_speed + current_motor_b_speed) / 2;
    last_speeds[speed_head++] = current_robot_speed;
    if(speed_head >= sizeof(last_speeds)/sizeof(last_speeds[0])) speed_head = 0;

    mean_speed = 0;
    for(uint32_t i = 0; i < sizeof(last_speeds)/sizeof(last_speeds[0]); i++)
    {
        mean_speed += last_speeds[i];
    }
    mean_speed /= sizeof(last_speeds)/sizeof(last_speeds[0]);
    printf("%f,%f\n", current_robot_speed, mean_speed);
    motor_a_enc_last = motor_a_enc;
    motor_b_enc_last = motor_b_enc;

    #ifdef DEBUG_MODE
    // printf("%f\t%f\n",current_motor_a_speed, current_motor_b_speed);
    #endif
}
/*
w funkcji controler_update()
refresh_data()
pid_compute() // wszystkie pidy

*/

void controler_update()
{
    refresh_data();

    pid_compute(pid_speed);
    target_angle = zero_angle + target_angle_offset;
    pid_compute(pid_imu);
    pid_compute(pid_motor_a);
    pid_compute(pid_motor_b);
    motor_a_power = target_motors_speed * 0.1;
    motor_b_power = target_motors_speed * 0.1;
    motor_set_power(MOTOR_A,motor_a_power);
    motor_set_power(MOTOR_B,motor_b_power);
}

void controler_stop()
{
    pid_set_mode(pid_speed, E_MODE_MANUAL);
    pid_set_mode(pid_imu, E_MODE_MANUAL);
    pid_set_mode(pid_motor_a, E_MODE_MANUAL);
    pid_set_mode(pid_motor_b, E_MODE_MANUAL);
    *pid_imu->output = 0;
    motor_a_power = 0;
    motor_b_power = 0;
    // TODO: parking
}

void controler_run()
{
    pid_tune(pid_speed, pid_speed->kp_disp, pid_speed->ki_disp, pid_speed->kd_disp);
    pid_tune(pid_imu, pid_imu->kp_disp, pid_imu->ki_disp, pid_imu->kd_disp);
    pid_tune(pid_motor_a, pid_motor_a->kp_disp, pid_motor_a->ki_disp, pid_motor_a->kd_disp);
    pid_tune(pid_motor_b, pid_motor_a->kp_disp, pid_motor_a->ki_disp, pid_motor_a->kd_disp);

    pid_set_mode(pid_speed, E_MODE_AUTO);
    pid_set_mode(pid_imu, E_MODE_AUTO);
    // pid_set_mode(pid_motor_a, E_MODE_AUTO);
    // pid_set_mode(pid_motor_b, E_MODE_AUTO);

}

