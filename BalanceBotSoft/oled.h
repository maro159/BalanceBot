#ifndef _OLED_H_
#define _OLED_H_

#include <stdio.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "font.h"
#include "menu.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADR 0x3C

#define FONT font_8x5
#define FONT_WIDTH (FONT[1]+FONT[2])
#define FONT_HEIGHT (FONT[0])

void oled_init();
void oled_display_x(int y_pos);
void oled_show_menu(menu_t *menu);
void oled_show_value(float value, float step);
inline void oled_clear();
inline void oled_render();


#endif