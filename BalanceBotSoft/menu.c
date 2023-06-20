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


option_t menu_main_options[] =
{
    {"RUN", &menu_properties},
    {"PROPERTIES", &menu_properties},
    {"EXIT", NULL},
};

option_t menu_properties_options[] =
{
    {"VELOCITY", &menu_properties},
    {"ANGLE", &menu_properties},
    {"EXIT", &menu_main},
};

menu_t menu_main = {menu_main_options, sizeof(menu_main_options) / sizeof(menu_main_options[0])};
menu_t menu_properties = {menu_properties_options, sizeof(menu_properties_options) / sizeof(menu_properties_options[0])};

int set_v(set_t *current_values)
{
    int value = 0; 
    value = counter_en * current_values->step;
    if(value < current_values->min)
    {
        value = current_values->min;
    }
    else if(value > current_values->max)
    {
        value = current_values->max;
    }
    return value;
}