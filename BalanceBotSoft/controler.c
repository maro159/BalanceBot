#include "pico/stdlib.h"
#include "controler.h"
#include "pins.h"
#include "acc_gyro.h"
#include "pid.h"
#include "motor.h"

/*
w funkcji init_controler()
dodac zmienna const sampling_time_us

wywolac dla kazdego pid_t (nie zapisywac nigdzie tego co zwraca funkcja)
pid_create(pid_ctrl pid, float* in, float* out, float* set, float kp, float ki, float kd)
dodatkowo dla każdego:
void pid_sample(pid_ctrl pid, uint32_t new_sampletime_us)   -> na podstawie const sampling_time_us
pid_limits(pid_ctrl pid, float min, float max); -> to beda wartosci "z palca", albo zdefiniowane przez #define albo jakies consty
*/


float current_robot_speed = 0.0;
float target_robot_speed = 0.0; // to remote control

float target_angle = 0.0;

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


void init_controler()
{   
    float kp_speed = 0.0;
    float ki_speed = 0.0;
    float kd_speed = 0.0;

    float kp_imu = 0.2;
    float ki_imu = 0.0;
    float kd_imu = 0.0;

    float kp_motor_a = 0.1;
    float ki_motor_a = 0.0;
    float kd_motor_a = 0.0;
    
    float kp_motor_b = 0.1;
    float ki_motor_b = 0.0;
    float kd_motor_b = 0.0;

    pid_speed = pid_create(&pid_speed_ctrl, &current_robot_speed, &target_angle, &target_robot_speed, kp_speed, ki_speed, kd_speed); 
    pid_imu = pid_create(&pid_imu_ctrl, &current_angle, &target_motors_speed, &target_angle, kp_imu, ki_imu, kd_imu);
    pid_motor_a = pid_create(&pid_motor_a_ctrl,&current_motor_a_speed,&motor_a_power,&target_motors_speed, kp_motor_a, ki_motor_a, kd_motor_a);
    pid_motor_b = pid_create(&pid_motor_b_ctrl, &current_motor_b_speed, &motor_b_power, &target_motors_speed, kp_motor_b, ki_motor_b, kd_motor_b);

    

    pid_sample(pid_speed, sampling_time_us);
    pid_sample(pid_imu, sampling_time_us);
    pid_sample(pid_motor_a, sampling_time_us);
    pid_sample(pid_motor_b, sampling_time_us);

    pid_limits(pid_speed, -60, 60); //miny i maxy do wpisania ręcznie
    pid_limits(pid_imu, -10, 10);
    pid_limits(pid_motor_a, -1, 1);
    pid_limits(pid_motor_b, -1, 1);

}

/*dane potrzebne do pid_create()
            pid_speed                   pid_imu                 pid_motor_a                 pid_motor_b
in          &current_robot_speed        &current_angle          &current_motor_a_speed      &current_motor_b_speed
out         &target_angle               &target_motor_speed     &motor_a_power              &motor_a_power
set         &target_robotspeed          &target_angle           &target_motor_speed         &target_motor_speed

utworzyc wszystko jako float na poczatku pliku controller c i zainicjalizować wartością 0.0 


w funkcji refresh_data()
*current_angle = //wzor na filtr komplmentarny z uzyciem zmiennych globalnych z pliku acc_gyro i sampling_time_us (albo zamianka sampling_time_us na swiezo obliczony czas)
*current_robot_speed = ...
*current_motor_a_speed = ...

*/

void refresh_data() 
{
    float alpha = 0.995;

    motor_encoder_request();
    mpu6050_read_data();
    
    int32_t motor_a_enc = motor_encoder_get(MOTOR_A);
    int32_t motor_b_enc = motor_encoder_get(MOTOR_B);

    current_motor_a_speed = (motor_a_enc - motor_a_enc_last) / (MOTOR_ENCODER_TICKS * sampling_time_sec);
    current_motor_b_speed = (motor_b_enc - motor_b_enc_last) / (MOTOR_ENCODER_TICKS * sampling_time_sec);  


    current_angle = alpha * (current_angle + sampling_time_sec * gyro_angular) + (1 - alpha) * acc_angle_deg;
    // current_angle = (current_angle + sampling_time_sec * gyro_angular);    
    current_robot_speed = (current_motor_a_speed + current_motor_b_speed) / 2;

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
    pid_compute(pid_imu);
    pid_compute(pid_motor_a);
    pid_compute(pid_motor_b);
    #ifdef DEBUG_MODE
    printf("%f\t%f\t%f\t%f\n", *pid_imu->input,*pid_imu->output, *pid_motor_a->setpoint,*pid_motor_a->output);
    #endif
    motor_set_power(MOTOR_A,motor_a_power);
    motor_set_power(MOTOR_B,motor_b_power);
}

void controler_stop()
{
    pid_set_mode(pid_speed, E_MODE_MANUAL);
    pid_set_mode(pid_imu, E_MODE_MANUAL);
    pid_set_mode(pid_motor_a, E_MODE_MANUAL);
    pid_set_mode(pid_motor_b, E_MODE_MANUAL);
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
    pid_set_mode(pid_motor_a, E_MODE_AUTO);
    pid_set_mode(pid_motor_b, E_MODE_AUTO);
}

