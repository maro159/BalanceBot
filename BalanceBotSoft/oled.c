// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <ctype.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pins.h"
#include "oled.h"
#include "ssd1306.h"
#include "menu.h"
#include "font.h"
#include "logo.h"

static ssd1306_t oled;

void init_oled()
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
    const uint32_t max_chars = 20;
    uint32_t decimal_digits;
    if (step >= 1.0) decimal_digits = 0; // for int-like values
    else
    {
        float magnitude = log10f(fabsf(step));  // calculate number of decimal digits
        decimal_digits = (uint32_t)ceilf(-magnitude);
    }
    char valueString[max_chars];
    snprintf(valueString, max_chars, "% 1.*f", decimal_digits, value);  // convert number to string
    ssd1306_draw_string_with_font(&oled, 0, 2*FONT_HEIGHT, 1, FONT, valueString);
    ssd1306_show(&oled);
}

void oled_display_x(int32_t y_pos)
{ 
    for (int32_t i=0 ; i<OLED_HEIGHT/FONT_HEIGHT+1; i++) {
        ssd1306_draw_string_with_font(&oled, 0, i*FONT_HEIGHT, 1, FONT, " ");
    }
    ssd1306_draw_string_with_font(&oled, 0, y_pos*FONT_HEIGHT, 1, FONT, "*");    
    ssd1306_show(&oled);  
}

void oled_clear()
{
    ssd1306_clear(&oled);
}

void oled_show_logo()
{
    ssd1306_bmp_show_image(&oled, logo_bmp_data, logo_bmp_size);
    ssd1306_show(&oled);  
}