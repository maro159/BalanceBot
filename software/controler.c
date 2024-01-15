#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "controler.h"
#include "pins.h"
#include "imu.h"
#include "pid.h"
#include "motor.h"
#include "remote_control.h"
#include "filters.h"

static float sampling_time_sec;

static float acc_angle_deg = 0.0;
static float gyro_angular = 0.0;

static float in_speed[2] = {0};
static float out_speed[2] = {0};
// static float last_speeds[30] = {0};
// static uint32_t last_speeds_head = 0;

static float current_robot_speed = 0.0;
static float target_robot_speed = 0.0;
static float set_input[2] = {0};
static float set_output[2] = {0};

float zero_angle = 8.0; // global
static float target_angle = 0.0;
static float target_angle_offset = 0.0;

static float current_angle = 0.0;
static float target_motors_speed = 0.0;   

static float current_motor_a_speed = 0.0;
static float motor_a_power = 0.0;                

static float current_motor_b_speed = 0.0;
static float motor_b_power = 0.0;   

static struct pid_controller pid_speed_ctrl;
static struct pid_controller pid_imu_ctrl;
static struct pid_controller pid_motor_a_ctrl;
static struct pid_controller pid_motor_b_ctrl;

pid_ctrl pid_speed;
pid_ctrl pid_imu;
pid_ctrl pid_motor_a;
pid_ctrl pid_motor_b;

static int32_t motor_a_enc_last = 0; 
static int32_t motor_b_enc_last = 0; 

IIR_filter_t iir;
IIR_filter_t iir2;


/*
            pid_speed                   pid_imu                 pid_motor_a                 pid_motor_b
in          &current_robot_speed        &current_angle          &current_motor_a_speed      &current_motor_b_speed
out         &target_angle               &target_motor_speed     &motor_a_power              &motor_a_power
set         &target_robotspeed          &target_angle           &target_motor_speed         &target_motor_speed

*/

static void _refresh_data() 
{
    // TODO: make independent from sampling timer
    // parameter used in complementary filter. Pretty much dependent on sampling time.
    float alpha = 0.990;
    // float alpha = 0.990;
    try_get_remote_target_speed(&target_robot_speed);
    set_input[0] = target_robot_speed;
    Low_Pass_IIR_Filter(&iir2, set_output, set_input);
    target_robot_speed = set_output[0];
    motor_encoder_request();
    imu_get_data(&acc_angle_deg, &gyro_angular);
    
    // implementation of complementary filter to obtain reliable angle
    current_angle = alpha * (current_angle + sampling_time_sec * gyro_angular) + (1 - alpha) * acc_angle_deg;
    // current_angle = (current_angle + sampling_time_sec * gyro_angular);  // gyroscope-only angle 
    // current_angle = alpha * current_angle + (1 - alpha) * acc_angle_deg; // accelerometer-only angle

    int32_t motor_a_enc = motor_encoder_fetch(MOTOR_A);
    int32_t motor_b_enc = motor_encoder_fetch(MOTOR_B);

    /* unit of motor speed is 4 * rps (rotates per second)
     * could be divided by 4 but is not
     */
    current_motor_a_speed = (motor_a_enc - motor_a_enc_last) / (MOTOR_ENCODER_TICKS * sampling_time_sec);
    current_motor_b_speed = (motor_b_enc - motor_b_enc_last) / (MOTOR_ENCODER_TICKS * sampling_time_sec);
    #if ENC_A_INVERT==1
    current_motor_a_speed = -current_motor_a_speed;
    #endif
    #if ENC_B_INVERT==1
    current_motor_b_speed = -current_motor_b_speed;
    #endif

    /* robot speed is calculated as moving average to achieve smooth control */
    current_robot_speed = (current_motor_a_speed + current_motor_b_speed) / 2.0;
    in_speed[0] = current_robot_speed;
    Low_Pass_IIR_Filter(&iir, out_speed, in_speed);
    current_robot_speed = out_speed[0];

    // last_speeds[last_speeds_head++] = current_robot_speed;
    // if(last_speeds_head >= sizeof(last_speeds)/sizeof(last_speeds[0])) last_speeds_head = 0;

    // current_robot_speed = 0;
    // for(uint32_t i = 0; i < sizeof(last_speeds)/sizeof(last_speeds[0]); i++)
    // {
    //     current_robot_speed += last_speeds[i];
    // }
    // current_robot_speed /= sizeof(last_speeds)/sizeof(last_speeds[0]);
    motor_a_enc_last = motor_a_enc;
    motor_b_enc_last = motor_b_enc;
}

void init_controler(uint32_t sampling_time_us)
{   
    if(!recursive_mutex_is_initialized(&remote_control_mutex))
    {
        recursive_mutex_init(&remote_control_mutex);
    }
    sampling_time_sec = sampling_time_us / (1000000.0);
    float kp_speed = 3.0;   // 2.3
    float ki_speed = 2.0;   // 2.8
    float kd_speed = 0.040;  // 0.09

    float kp_imu = 5.2;
    float ki_imu = 0.0;
    float kd_imu = 0.085;

    float kp_motor = 0.10;
    float ki_motor = 0.07;
    float kd_motor = 0.00;

    pid_speed = pid_create(&pid_speed_ctrl, &current_robot_speed, &target_angle_offset, &target_robot_speed, kp_speed, ki_speed, kd_speed); 
    pid_imu = pid_create(&pid_imu_ctrl, &current_angle, &target_motors_speed, &target_angle, kp_imu, ki_imu, kd_imu);
    pid_motor_a = pid_create(&pid_motor_a_ctrl, &current_motor_a_speed, &motor_a_power, &target_motors_speed, kp_motor, ki_motor, kd_motor);
    pid_motor_b = pid_create(&pid_motor_b_ctrl, &current_motor_b_speed, &motor_b_power, &target_motors_speed, kp_motor, ki_motor, kd_motor);

    pid_sample(pid_speed, sampling_time_us);
    pid_sample(pid_imu, sampling_time_us);
    pid_sample(pid_motor_a, sampling_time_us);
    pid_sample(pid_motor_b, sampling_time_us);

    pid_limits(pid_speed, -15, 15); // max angle offset to achieve speed
    pid_limits(pid_imu, -10, 10);   // max motor speed
    pid_limits(pid_motor_a, -1, 1); // max motor a power
    pid_limits(pid_motor_b, -1, 1); // max motor b power

    _refresh_data();
    current_angle = acc_angle_deg;  // initial angle from accelerometer

    // low pass to filter robot speed
    iir.samplingTime = sampling_time_sec;
    iir.tau = 0.12;
    iir2.samplingTime = sampling_time_sec;
    iir2.tau = 0.7;
    Low_Pass_IIR_Filter_Init(&iir);
    Low_Pass_IIR_Filter_Init(&iir2);
}
 
void controler_update()
{
    _refresh_data();
    // disable controler if falled
    int32_t current_angle_int = (int32_t)(current_angle);
    if(current_angle_int > 45 || current_angle_int < -45) controler_stop();

    pid_compute(pid_speed);     // compute angle offset to achieve target speed
    target_angle = zero_angle - target_angle_offset;
    pid_compute(pid_imu);       // compute motor speed to achieve target angle 
    pid_compute(pid_motor_a);   // compute motor power (pwm) to achieve target speed
    pid_compute(pid_motor_b);   // compute motor power (pwm) to achieve target speed
    motor_a_power = target_motors_speed * 0.1;  // TODO: use pid instead
    motor_b_power = target_motors_speed * 0.1;  // TODO: use pid instead
    motor_set_power(MOTOR_A,motor_a_power);
    motor_set_power(MOTOR_B,motor_b_power);
}

void controler_stop()
{
    // disable pids, and set outputs to 0
    pid_set_mode(pid_speed, E_MODE_MANUAL);
    pid_set_mode(pid_imu, E_MODE_MANUAL);
    pid_set_mode(pid_motor_a, E_MODE_MANUAL);
    pid_set_mode(pid_motor_b, E_MODE_MANUAL);
    *pid_speed->output = 0;
    *pid_imu->output = 0;
    *pid_motor_a->output = 0;
    *pid_motor_b->output = 0;
    // TODO: parking
}

void controler_run()
{
    Low_Pass_IIR_Filter_Init(&iir);
    // set pids coeffs which may changed in menu
    pid_tune(pid_speed, pid_speed->kp_disp, pid_speed->ki_disp, pid_speed->kd_disp);
    pid_tune(pid_imu, pid_imu->kp_disp, pid_imu->ki_disp, pid_imu->kd_disp);
    pid_tune(pid_motor_a, pid_motor_a->kp_disp, pid_motor_a->ki_disp, pid_motor_a->kd_disp);
    pid_tune(pid_motor_b, pid_motor_a->kp_disp, pid_motor_a->ki_disp, pid_motor_a->kd_disp);

    // turn on pids
    pid_set_mode(pid_speed, E_MODE_AUTO);
    pid_set_mode(pid_imu, E_MODE_AUTO);
    pid_set_mode(pid_motor_a, E_MODE_AUTO);
    pid_set_mode(pid_motor_b, E_MODE_AUTO);
}

