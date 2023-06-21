#include <stdio.h>
// #include <string.h>
#include <stdlib.h>
#include <math.h>
// #include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "raspberry26x32.h"
#include "ssd1306_font.h"
#include "pins.h"
#include "menu.h"
#include "oled.h"
#include "encoder_rot.h"

option_t menu_main_options[] =
{
    {"RUN", &menu_properties},
    {"PROPERTIES", &menu_properties},
    {"EXIT", NULL},
};

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

menu_t menu_main = {MENU_NORMAL, menu_main_options, {0, MENU_SIZE(menu_main_options)-1, 1}};
menu_t menu_properties = {MENU_NORMAL, menu_properties_options, {0, MENU_SIZE(menu_properties_options)-1, 1}};
menu_t menu_velocity = {MENU_OPTION, menu_velocity_options, {-5.5,5.5,0.1}};

void menu_init()
{
    current_menu = &menu_main;
    encoder_set_limit(current_menu->limits.min, current_menu->limits.max);
    oled_show_menu(current_menu);
    oled_display_x(0);
    
}

void menu_bind_parameter(void *param, option_t *option)
{
    option[0].ptr = param;
}

int menu_execute()
{
    if(encoder_changed()) 
        {
            // printf("%d/%d", last_count, counter_en, last_count);
            if(current_menu->menu_type == MENU_NORMAL)
            {
                oled_display_x(encoder_get());
            }
            else if (current_menu->menu_type == MENU_OPTION )
            {   
                current_value += encoder_get() * current_menu->limits.step;
                oled_show_values(current_value);
            }
        }

        if(encoder_clicked())
        {
            menu_t *nextMenu;
            if(current_menu->menu_type == MENU_NORMAL)
            {
                nextMenu = current_menu->options[encoder_get()].ptr;
                if(nextMenu == NULL) {oled_clear(); return 1;} // EXIT 

                if(nextMenu->menu_type == MENU_OPTION)
                {
                    current_value = (*(float*)(nextMenu->options[0].ptr));    // get initial value of parameter
                    int enc_min = roundf((-abs(current_value - nextMenu->limits.min))/(nextMenu->limits.step)); // calc limit for encoder
                    int enc_max = roundf((abs(current_value - nextMenu->limits.max))/(nextMenu->limits.step)); 
                    encoder_set_limit(enc_min, enc_max);    // TODO: check if currentValue > min && currentValue < max
                    encoder_set(0);
                }
                else if (nextMenu->menu_type == MENU_NORMAL)
                {
                    encoder_set_limit(nextMenu->limits.min, nextMenu->limits.max);
                    encoder_set(nextMenu->limits.min);  // to allow blocked menu options
                }
            }
            else if(current_menu->menu_type == MENU_OPTION)
            {
                nextMenu = current_menu->options[1].ptr;            // pointer to higher level menu
                (*(float*)(current_menu->options[0].ptr)) = current_value;    // save value of parameter
                encoder_set_limit(nextMenu->limits.min, nextMenu->limits.max);
                encoder_set(nextMenu->limits.min);  // to allow blocked menu options
                printf("parameter: %3f", current_value);    // TODO: temporary
            }

            oled_clear();
            oled_show_menu(nextMenu);
            current_menu = nextMenu;
        }
        return 0;
}