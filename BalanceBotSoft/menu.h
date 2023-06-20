#ifndef _MENU_H_
#define _MENU_H_
#include "pico/stdlib.h"

typedef struct option_t
{
    char* name;
    void* ptr;
} option_t;

typedef struct menu_t
{
    option_t *options;
    uint32_t count;
} menu_t;

typedef struct set_t
{
    int min;
    int max;
    int step;

} set_t;

int set_v(set_t *current_values);
extern option_t menu_main_options[];
extern option_t menu_properties_options[];
extern menu_t menu_main;
extern menu_t menu_properties;

// extern menu_t menu_main;
// extern menu_t menu_properties;

#endif
