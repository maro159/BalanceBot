#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "pins.h"
#include "menu.h"
#include "oled.h"
#include "motor.h"
#include "controler.h"
#include "encoder_rot.h"

static menu_t *current_menu;
static float initial_param;
static float new_param;

/* NORMAL MENUS */
menu_t menu_run;
menu_t menu_main;
menu_t menu_settings;
menu_t menu_pids;
menu_t menu_pid_speed;
menu_t menu_pid_imu;
menu_t menu_pid_motor;

/* PARAMETER MENUS */
menu_t menu_pid_speed_kp;
menu_t menu_pid_speed_ki;
menu_t menu_pid_speed_kd;
menu_t menu_pid_imu_kp;
menu_t menu_pid_imu_ki;
menu_t menu_pid_imu_kd;
menu_t menu_pid_motor_kp;
menu_t menu_pid_motor_ki;
menu_t menu_pid_motor_kd;
menu_t menu_motor_power;
menu_t menu_angle;
menu_t menu_iir_tau;

/* NORMAL MENU OPTIONS 
 * each option in array should have format {string, void*}
 * string is displayed name of option
 * pointer should point to submenu   
 * IMPORTANT: First option should be name of this menu, and pointer to itself
 */

static option_t menu_run_options[] =
{
    {"CLICK TO STOP...", &menu_main},
    {"", &menu_main},
};
static option_t menu_main_options[] =
{
    {"** MAIN MENU **", &menu_main},
    {"RUN", &menu_run},
    {"SETTINGS", &menu_settings},
};
static option_t menu_settings_options[] =
{
    {"** SETTINGS **", &menu_settings},
    {"PIDs", &menu_pids},
    {"iir tau", &menu_iir_tau },
    {"motor power", &menu_motor_power},
    {"zero angle", &menu_angle},
    {"EXIT", &menu_main},
};
static option_t menu_pids_options[] =
{
    {"** PIDs **", &menu_pids},
    {"PID - speed", &menu_pid_speed},
    {"PID - imu", &menu_pid_imu},
    {"PID - motor", &menu_pid_motor},
    {"EXIT", &menu_settings},
};
static option_t menu_pid_speed_options[] =
{
    {"** PID - speed **", &menu_pid_speed},
    {"speed kp", &menu_pid_speed_kp},
    {"speed ki", &menu_pid_speed_ki},
    {"speed kd", &menu_pid_speed_kd},
    {"EXIT", &menu_pids},
};
static option_t menu_pid_imu_options[] =
{
    {"** PID - imu **", &menu_pid_imu},
    {"imu kp", &menu_pid_imu_kp},
    {"imu ki", &menu_pid_imu_ki},
    {"imu kd", &menu_pid_imu_kd},
    {"EXIT", &menu_pids},
};
static option_t menu_pid_motor_options[] =
{
    {"** PID - motor **", &menu_pid_motor},
    {"motor kp", &menu_pid_motor_kp},
    {"motor ki", &menu_pid_motor_ki},
    {"motor kd", &menu_pid_motor_kd},
    {"EXIT", &menu_pids},
};

static void _create_menu_param(menu_t *menu, const char *param_name, float *param, float min, float max, float step)
{
    // allocate memory for options array
    menu->options = (option_t*)malloc(2 * sizeof(option_t));
    // check if memory allocated
    if(menu->options != NULL) 
    {
        strncpy(menu->options[0].name, param_name, sizeof(menu->options[0].name) - 1);  // copy parameter name
        menu->options[0].ptr = param;               // pointer to actual parameter
        menu->options[1].name[0] = '\0';            // must be empty string
        menu->options[1].ptr = NULL;                // place for pointer to upper-level menu  
        // initialize menu fields
        menu->menu_type = MENU_PARAM;
        menu->limits.min = min;
        menu->limits.max = max;
        menu->limits.step = step;
    }
}

static void _create_menu_normal(menu_t *menu, option_t *options, size_t options_count)
{
    // initialize menu fields
    menu->menu_type = MENU_NORMAL;
    menu->options = options;
    menu->limits.min = 1;   // must be 1 because 0 is title of menu
    menu->limits.max = options_count - 1;
    menu->limits.step = 1;  // doesn't matter in normal menu anyway

    // special case for empty menus (title only)
    if(options_count <= 1)
    {
        menu->limits.min = 0;
        menu->limits.max = 0;
    }

    // assign this menu as parent for each MENU_PARAM submenu
    for (size_t i = 0; i < options_count; i++)  // iterate through each option of this menu
    {
        menu_t *submenu = (menu_t*)(menu->options[i].ptr); // get pointed submenu
        if (submenu != NULL)    // check if submenu is created
        {
            if(submenu->menu_type == MENU_PARAM) // execute only for parameter menu
            {
                submenu->options[1].ptr = menu; // assign this menu as parent
            }
        }
    }
}

void init_menu()
{
    // Parameter menus need to be created first to correctly assign parents to them.
    _create_menu_param(&menu_motor_power, "motor power", &motor_power_ratio, 0, 1, 0.1);
    _create_menu_param(&menu_pid_speed_kp, "speed kp", &pid_speed->kp_disp, 0, 30, 0.1);
    _create_menu_param(&menu_pid_speed_ki, "speed ki", &pid_speed->ki_disp, 0, 30, 0.1);
    _create_menu_param(&menu_pid_speed_kd, "speed kd", &pid_speed->kd_disp, 0, 1, 0.001);
    _create_menu_param(&menu_pid_imu_kp, "imu kp", &pid_imu->kp_disp, 0, 30, 0.1);
    _create_menu_param(&menu_pid_imu_ki, "imu ki", &pid_imu->ki_disp, 0, 30, 0.1);
    _create_menu_param(&menu_pid_imu_kd, "imu kd", &pid_imu->kd_disp, 0, 1, 0.001);
    _create_menu_param(&menu_pid_motor_kp, "motor kp", &pid_motor_a->kp_disp, 0, 3, 0.01);
    _create_menu_param(&menu_pid_motor_ki, "motor ki", &pid_motor_a->ki_disp, 0, 3, 0.01);
    _create_menu_param(&menu_pid_motor_kd, "motor kd", &pid_motor_a->kd_disp, 0, 1, 0.001);
    _create_menu_param(&menu_angle, "zero angle", &zero_angle, -15, 15, 0.1);
    _create_menu_param(&menu_iir_tau, "iir speed tau", &iir.tau, 0, 1, 0.01);
    _create_menu_normal(&menu_pid_motor, menu_pid_motor_options, OPTIONS_COUNT(menu_pid_motor_options));
    _create_menu_normal(&menu_pid_imu, menu_pid_imu_options, OPTIONS_COUNT(menu_pid_imu_options));
    _create_menu_normal(&menu_pid_speed, menu_pid_speed_options, OPTIONS_COUNT(menu_pid_speed_options));
    _create_menu_normal(&menu_pids, menu_pids_options, OPTIONS_COUNT(menu_pids_options));
    _create_menu_normal(&menu_settings, menu_settings_options, OPTIONS_COUNT(menu_settings_options));
    _create_menu_normal(&menu_run, menu_run_options, OPTIONS_COUNT(menu_run_options));
    _create_menu_normal(&menu_main, menu_main_options, OPTIONS_COUNT(menu_main_options));    

    current_menu = &menu_main;
    encoder_limit(current_menu->limits.min, current_menu->limits.max);
    oled_show_menu(current_menu);
    encoder_set(current_menu->limits.min);
    oled_display_cursor(current_menu->limits.min);
}

menu_t *menu_get()
{
    if(encoder_changed()) 
    {
        if(current_menu->menu_type == MENU_NORMAL)
        {
            oled_display_cursor(encoder_get()); // display cursor to select submenu
        }
        else if (current_menu->menu_type == MENU_PARAM )
        {   
            new_param = initial_param + (encoder_get()) * current_menu->limits.step;    // calculate new value of parameter
            oled_show_value(new_param, current_menu->limits.step);  // display new value on display
        }
    }

    if(encoder_clicked())
    {
        menu_t *nextMenu;
        if(current_menu->menu_type == MENU_NORMAL)
        {
            nextMenu = (menu_t*)(current_menu->options[encoder_get()].ptr); // choose menu based on encoder selection
        }
        else if(current_menu->menu_type == MENU_PARAM)
        {
            nextMenu = (menu_t*)(current_menu->options[1].ptr);            // pointer to higher level menu
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
            oled_display_cursor(nextMenu->limits.min); // TODO: move it to better place
        }
        else if(nextMenu->menu_type == MENU_PARAM)
        {
            initial_param = (*(float*)(nextMenu->options[0].ptr));    // get initial value of parameter
            int32_t enc_min = -floorf(fabsf((initial_param - nextMenu->limits.min)/(nextMenu->limits.step))); // calc limit for encoder
            int32_t enc_max = floorf(fabsf((initial_param - nextMenu->limits.max)/(nextMenu->limits.step)));
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
    // if(current_menu == &menu_run) return true;
    // else return false;
    return current_menu;
}