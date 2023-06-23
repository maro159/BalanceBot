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

static menu_t *current_menu; // TODO: is static ok?
static float initial_param;
static float new_param;

option_t menu_main_options[] =
{
    {"RUN", &menu_properties},
    {"PROPERTIES", &menu_properties},
    {"EXIT", NULL},
};

option_t menu_properties_options[] =
{
    {"VELOCITY", &menu_velocity},
    {"ANGLE", &menu_angle},
    {"EXIT", &menu_main},
};

option_t menu_velocity_options[] =
{
    {"VELOCITY", NULL}, // tutaj wskaznik do zapisu/odczytu parametru
    {"", &menu_properties}, // tutaj wskaznik do menu poziom wyzej
};

option_t menu_angle_options[] =
{
    {"ANGLE", NULL}, // tutaj wskaznik do zapisu/odczytu parametru
    {"", &menu_properties}, // tutaj wskaznik do menu poziom wyzej
};

menu_t menu_main = {MENU_NORMAL, menu_main_options, {0, MENU_SIZE(menu_main_options)-1, 1}};
menu_t menu_properties = {MENU_NORMAL, menu_properties_options, {0, MENU_SIZE(menu_properties_options)-1, 1}};
menu_t menu_velocity = {MENU_OPTION, menu_velocity_options, {-5.5,5.5,0.1}};
menu_t menu_angle = {MENU_OPTION, menu_angle_options, {-180,180,1}};

void menu_init()
{
    current_menu = &menu_main;
    encoder_limit(current_menu->limits.min, current_menu->limits.max);
    oled_show_menu(current_menu);
    oled_display_x(0);
}

void menu_bind_parameter(void *param, menu_t *menu)
{
    menu->options[0].ptr = param;
}

int32_t menu_execute()
{
    if(encoder_changed()) 
        {
            if(current_menu->menu_type == MENU_NORMAL)
            {
                oled_display_x(encoder_get());
            }
            else if (current_menu->menu_type == MENU_OPTION )
            {   
                new_param = initial_param + (encoder_get()) * current_menu->limits.step;
                oled_show_value(new_param, current_menu->limits.max);
            }
        }

        if(encoder_clicked())
        {
            menu_t *nextMenu;
            if(current_menu->menu_type == MENU_NORMAL)
            {
                nextMenu = current_menu->options[encoder_get()].ptr; // choose menu based on encoder selection
            }
            else if(current_menu->menu_type == MENU_OPTION)
            {
                nextMenu = current_menu->options[1].ptr;            // pointer to higher level menu
                (*(float*)(current_menu->options[0].ptr)) = new_param;    // save value of parameter
                #ifdef DEBUG_MODE
                printf("saved: %3f\n", new_param);    // TODO: temporary
                #endif
            }
            if(nextMenu == NULL) {oled_clear(); return 1;} // EXIT )
            
            oled_clear();
            oled_show_menu(nextMenu);

            if(nextMenu->menu_type == MENU_NORMAL)
            {
                encoder_limit(nextMenu->limits.min, nextMenu->limits.max);
                encoder_set(nextMenu->limits.min);  // to allow blocked menu options
                oled_display_x(nextMenu->limits.min); // TODO: move it to better place
            }
            else if(nextMenu->menu_type == MENU_OPTION)
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
        return 0;
}