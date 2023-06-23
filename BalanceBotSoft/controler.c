#include "pico/stdlib.h"
#include "controler.h"
#include "pins.h"
#include "acc_gyro.h"

/*
w funkcji controler_init()
dodac zmienna const sampling_time_us

wywolac dla kazdego pid_t (nie zapisywac nigdzie tego co zwraca funkcja)
pid_create(pid_ctrl pid, float* in, float* out, float* set, float kp, float ki, float kd)
dodatkowo dla każdego:
void pid_sample(pid_ctrl pid, uint32_t new_sampletime_us)   -> na podstawie const sampling_time_us
pid_limits(pid_ctrl pid, float min, float max); -> to beda wartosci "z palca", albo zdefiniowane przez #define albo jakies consty


dane potrzebne do pid_create()
            pid_speed                   pid_imu                 pid_motor_a                 pid_motor_b
in          &current_robot_speed        &current_angle          &current_motor_a_speed      &current_motor_b_speed
out         &target_angle               &target_motor_power     &motor_a_power              &motor_a_power
set         &target_robotspeed          &target_angle           &target_motor_power         &target_motor_power

utworzyc wszystko jako float na poczatku pliku controller c i zainicjalizować wartością 0.0 


w funkcji refresh_data()
*current_angle = //wzor na filtr komplmentarny z uzyciem zmiennych globalnych z pliku acc_gyro i sampling_time_us (albo zamianka sampling_time_us na swiezo obliczony czas)
*current_robot_speed = ...
*current_motor_a_speed = ...

w funkcji controler_update()
refresh_data()
pid_compute() // wszystkie pidy

*/

pid_t pid_speed;
pid_t pid_imu;
pid_t pid_motor_a;
pid_t pid_motor_b;
