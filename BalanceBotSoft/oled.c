// #include <stdio.h>
// #include <string.h>
#include <stdlib.h>
// #include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pins.h"
#include "oled.h"
#include "ssd1306.h"
#include "menu.h"
#include "oled.h"
#include "encoder_rot.h"

ssd1306_t oled;

void oled_init()
{
    #if !defined(OLED_I2C) || !defined(OLED_SDA) || !defined(OLED_SCL)
    #warning i2c / SSD1306_i2d example requires a board with I2C pins
        puts("Default I2C pins were not defined");
    #else
    ssd1306_init(&oled, OLED_WIDTH, OLED_HEIGHT, OLED_ADR, OLED_I2C);
    oled.external_vcc = true;
    ssd1306_clear(&oled);
    #endif
}

void oled_show_menu(menu_t *menu)
{    
    int32_t limit = 1;
    if(menu->menu_type == MENU_NORMAL) limit = menu->limits.max + 1;
    for (int32_t i = 0; i < limit; i++) {
        ssd1306_draw_string_with_font(&oled, 2*FONT_WIDTH, i*FONT_HEIGHT, 1, FONT, menu->options[i].name);
    }
    ssd1306_show(&oled);
}

void oled_show_value(float value, float step)
{
    char *format = "";              // format for ints
    if(step != 1.0) *format = "";   // format for floats
    char valueString[16];  
    sprintf(valueString, format, value);  
    ssd1306_draw_string_with_font(&oled, 0, 2*FONT_HEIGHT, 1, FONT, valueString);
    ssd1306_show(&oled);
}

void oled_display_x(uint32_t y_pos)
{ 
    for (int32_t i = 0 ; i < OLED_HEIGHT/FONT_HEIGHT + 1; i++) {
        ssd1306_draw_string_with_font(&oled, 0, i*FONT_HEIGHT, 1, FONT, " ");
    }
    ssd1306_draw_string_with_font(&oled, 0, y_pos*FONT_HEIGHT, 1, FONT, "*");    
    ssd1306_show(&oled);  
}

void oled_clear()
{
    ssd1306_clear(&oled);
}