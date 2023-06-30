#include <stdio.h>
// #include <string.h>
#include <stdlib.h>
#include <math.h>
// #include <ctype.h>
#include "pico/stdlib.h"
#include "pins.h"
#include "menu.h"
#include "oled.h"
#include "motor.h"
#include "controler.h"
#include "encoder_rot.h"

static menu_t *current_menu; // TODO: is static ok?
static float initial_param;
static float new_param;

/* NORMAL MENU OPTIONS */

option_t menu_run_options[] =
{
    {"CLICK TO STOP...", &menu_main},
};
option_t menu_main_options[] =
{
    {"** MAIN MENU **", &menu_main},
    {"RUN", &menu_run},
    {"SETTINGS", &menu_settings},
};
option_t menu_settings_options[] =
{
    {"** SETTINGS **", &menu_settings},
    {"PIDs", &menu_pids},
    {"motor power", &menu_motor_power},
    {"zero angle", &menu_angle},
    {"EXIT", &menu_main},
};
option_t menu_pids_options[] =
{
    {"** PIDs **", &menu_pids},
    {"PID - speed", &menu_pid_speed},
    {"PID - imu", &menu_pid_imu},
    {"PID - motor", &menu_pid_motor},
    {"EXIT", &menu_settings},
};
option_t menu_pid_speed_options[] =
{
    {"** PID - speed **", &menu_pid_speed},
    {"speed kp", &menu_pid_speed_kp},
    {"speed ki", &menu_pid_speed_ki},
    {"speed kd", &menu_pid_speed_kd},
    {"EXIT", &menu_pids},
};
option_t menu_pid_imu_options[] =
{
    {"** PID - imu **", &menu_pid_imu},
    {"imu kp", &menu_pid_imu_kp},
    {"imu ki", &menu_pid_imu_ki},
    {"imu kd", &menu_pid_imu_kd},
    {"EXIT", &menu_pids},
};
option_t menu_pid_motor_options[] =
{
    {"** PID - motor **", &menu_pid_motor},
    {"motor kp", &menu_pid_motor_kp},
    {"motor ki", &menu_pid_motor_ki},
    {"motor kd", &menu_pid_motor_kd},
    {"EXIT", &menu_pids},
};

/* PARAM MENU OPTIONS */
option_t menu_pid_speed_kp_options[] =
{
    {"speed kp", NULL}, // pointer to parameter to be changed
    {"", &menu_pid_speed}, // pointer to upper level menu
};
option_t menu_pid_speed_ki_options[] =
{
    {"speed ki", NULL},
    {"", &menu_pid_speed},
};
option_t menu_pid_speed_kd_options[] =
{
    {"speed kd", NULL},
    {"", &menu_pid_speed},
};
option_t menu_pid_imu_kp_options[] =
{
    {"imu kp", NULL},
    {"", &menu_pid_imu},
};
option_t menu_pid_imu_ki_options[] =
{
    {"imu ki", NULL},
    {"", &menu_pid_imu},
};
option_t menu_pid_imu_kd_options[] =
{
    {"imu kd", NULL},
    {"", &menu_pid_imu},
};
option_t menu_pid_motor_kp_options[] =
{
    {"motor kp", NULL},
    {"", &menu_pid_motor},
};
option_t menu_pid_motor_ki_options[] =
{
    {"motor ki", NULL},
    {"", &menu_pid_motor},
};
option_t menu_pid_motor_kd_options[] =
{
    {"motor kd", NULL},
    {"", &menu_pid_motor},
};

option_t menu_motor_power_options[] =
{
    {"motor power", NULL},
    {"", &menu_settings},
};
option_t menu_angle_options[] =
{
    {"zero angle", NULL},
    {"", &menu_settings},
};

menu_t menu_run = {MENU_NORMAL, menu_run_options, {0, MENU_SIZE(menu_run_options)-1, 0}};
menu_t menu_main = {MENU_NORMAL, menu_main_options, {1, MENU_SIZE(menu_main_options)-1, 1}};
menu_t menu_settings = {MENU_NORMAL, menu_settings_options, {1, MENU_SIZE(menu_settings_options)-1, 1}};
menu_t menu_pids = {MENU_NORMAL, menu_pids_options, {1, MENU_SIZE(menu_settings_options)-1, 1}};
menu_t menu_pid_speed = {MENU_NORMAL, menu_pid_speed_options, {1, MENU_SIZE(menu_settings_options)-1, 1}};
menu_t menu_pid_imu = {MENU_NORMAL, menu_pid_imu_options, {1, MENU_SIZE(menu_settings_options)-1, 1}};
menu_t menu_pid_motor = {MENU_NORMAL, menu_pid_motor_options, {1, MENU_SIZE(menu_settings_options)-1, 1}};

menu_t menu_pid_speed_kp = {MENU_PARAM, menu_pid_speed_kp_options, {-30, 30, 0.01}};
menu_t menu_pid_speed_ki = {MENU_PARAM, menu_pid_speed_ki_options, {-1, 1, 0.01}};
menu_t menu_pid_speed_kd = {MENU_PARAM, menu_pid_speed_kd_options, {-15, 15, 0.01}};

menu_t menu_pid_imu_kp = {MENU_PARAM, menu_pid_imu_kp_options, {-0, 30, 0.1}};
menu_t menu_pid_imu_ki = {MENU_PARAM, menu_pid_imu_ki_options, {0, 5, 0.01}};
menu_t menu_pid_imu_kd = {MENU_PARAM, menu_pid_imu_kd_options, {0, 5, 0.01}};

menu_t menu_pid_motor_kp = {MENU_PARAM, menu_pid_motor_kp_options, {-10, 10, 0.01}};
menu_t menu_pid_motor_ki = {MENU_PARAM, menu_pid_motor_ki_options, {-1, 1, 0.01}};
menu_t menu_pid_motor_kd = {MENU_PARAM, menu_pid_motor_kd_options, {-10, 10, 0.01}};

menu_t menu_motor_power = {MENU_PARAM, menu_motor_power_options, {0, 1, 0.01}};
menu_t menu_angle = {MENU_PARAM, menu_angle_options, {-10, 10, 0.1}};

void init_menu()
{
    menu_bind_parameter(&motor_power_ratio, &menu_motor_power);
    menu_bind_parameter(&pid_speed->kp_disp, &menu_pid_speed_kp);
    menu_bind_parameter(&pid_speed->ki_disp, &menu_pid_speed_ki);
    menu_bind_parameter(&pid_speed->kd_disp, &menu_pid_speed_kd);
    menu_bind_parameter(&pid_imu->kp_disp, &menu_pid_imu_kp);
    menu_bind_parameter(&pid_imu->ki_disp, &menu_pid_imu_ki);
    menu_bind_parameter(&pid_imu->kd_disp, &menu_pid_imu_kd);
    menu_bind_parameter(&pid_motor_a->kp_disp, &menu_pid_motor_kp);
    menu_bind_parameter(&pid_motor_a->ki_disp, &menu_pid_motor_ki);
    menu_bind_parameter(&pid_motor_a->kd_disp, &menu_pid_motor_kd);
    menu_bind_parameter(&zero_angle, &menu_angle);

    current_menu = &menu_main;
    encoder_limit(current_menu->limits.min, current_menu->limits.max);
    oled_show_menu(current_menu);
    encoder_set(current_menu->limits.min);
    oled_display_x(current_menu->limits.min);
}

void menu_bind_parameter(void *param, menu_t *menu)
{
    menu->options[0].ptr = param;
}

bool menu_execute()
{
    if(encoder_changed()) 
        {
            if(current_menu->menu_type == MENU_NORMAL)
            {
                oled_display_x(encoder_get());
            }
            else if (current_menu->menu_type == MENU_PARAM )
            {   
                new_param = initial_param + (encoder_get()) * current_menu->limits.step;
                oled_show_value(new_param, current_menu->limits.step);
            }
        }

        if(encoder_clicked())
        {
            menu_t *nextMenu;
            if(current_menu->menu_type == MENU_NORMAL)
            {
                nextMenu = current_menu->options[encoder_get()].ptr; // choose menu based on encoder selection
            }
            else if(current_menu->menu_type == MENU_PARAM)
            {
                nextMenu = current_menu->options[1].ptr;            // pointer to higher level menu
                (*(float*)(current_menu->options[0].ptr)) = new_param;    // save value of parameter
                #ifdef DEBUG_MODE
                printf("saved: %3f\n", new_param);    // TODO: temporary
                #endif
            }
            // if(nextMenu == NULL) {oled_clear(); return 1;} // EXIT )
            
            oled_clear();
            if(nextMenu == &menu_run) oled_show_logo();
            oled_show_menu(nextMenu);

            if(nextMenu->menu_type == MENU_NORMAL)
            {
                encoder_limit(nextMenu->limits.min, nextMenu->limits.max);
                encoder_set(nextMenu->limits.min);  // to allow blocked menu options
                oled_display_x(nextMenu->limits.min); // TODO: move it to better place
            }
            else if(nextMenu->menu_type == MENU_PARAM)
            {
                initial_param = (*(float*)(nextMenu->options[0].ptr));    // get initial value of parameter
                int32_t enc_min = roundf(-abs((initial_param - nextMenu->limits.min)/(nextMenu->limits.step))); // calc limit for encoder
                int32_t enc_max = roundf(abs((initial_param - nextMenu->limits.max)/(nextMenu->limits.step)));
                #ifdef DEBUG_MODE
                printf("enc min: %d\t enc max: %d\n", enc_min, enc_max);
                #endif
                encoder_limit(enc_min, enc_max);    // TODO: check if currentValue > min && currentValue < max
                encoder_set(0);
                oled_show_value(new_param, current_menu->limits.max);
            }
            else {}
            current_menu = nextMenu;
        }
        if(current_menu == &menu_run) return true;
        else return false;
}