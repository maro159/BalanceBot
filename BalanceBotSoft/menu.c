#include "menu.h"
#include "oled.h"
#include "encoder_rot.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "raspberry26x32.h"
#include "ssd1306_font.h"
#include "pins.h"
#include "encoder_rot.h"


option_t menu_properties_options[] =
{
    {"VELOCITY", &menu_velocity},
    {"ANGLE", &menu_properties},
    {"EXIT", &menu_main},
};

option_t menu_velocity_options[] =
{
    {"VELOCITY", NULL}, // tutaj wskaznik do zapisu/odczytu parametru
    {"", &menu_properties}, // tutaj wskaznik do menu poziom wyzej
};
option_t menu_main_options[] =
{
    {"RUN", &menu_properties},
    {"PROPERTIES", &menu_properties},
    {"EXIT", NULL},
};

limit_t limits_normal_menu_main =
{
    0,
    sizeof(menu_main_options) / sizeof(menu_main_options[0]),
    1

};

limit_t limits_normal_properties = 
{
    0,
    sizeof(menu_properties_options) / sizeof(menu_properties_options[0]),
    1
};
limit_t limits_option_velocity = 
{
    0,
    200,
    10
};

set_t test_values = 
{
    0,
    10,
    1
};

menu_t menu_main = {MENU_NORMAL, menu_main_options, &limits_normal_menu_main};
menu_t menu_properties = {MENU_NORMAL, menu_properties_options, &limits_normal_properties};
menu_t menu_velocity = {MENU_OPTION, menu_velocity_options, &limits_option_velocity};

int set_value(menu_t *current_menu)
{
    int value = counter_en * current_menu->limits->step;
    if(value < current_menu->limits->min)
    {
        value = current_menu->limits->min;
    }
    else if(value > current_menu->limits->max)
    {
        value = current_menu->limits->max;
    }
    return value;
    
}
