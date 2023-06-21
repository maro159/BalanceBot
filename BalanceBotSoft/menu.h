#ifndef _MENU_H_
#define _MENU_H_
#include "pico/stdlib.h"

typedef struct set_t
{
    int min;
    int max;
    int step;
} set_t;


typedef struct option_t
{
    char* name;
    void* ptr;
} option_t;


typedef enum menu_type_t
{
    MENU_NORMAL,
    MENU_OPTION,
} menu_type_t;

typedef struct limit_t
{
    int min;
    int max;
    int step;
    
} limit_t;

typedef struct menu_t
{
    menu_type_t *menu_type;
    option_t *options;
    limit_t *limits;
} menu_t;

int set_value(set_t *current_values);

extern option_t menu_main_options[];
extern option_t menu_properties_options[];

extern menu_t menu_main;
extern menu_t menu_properties;
extern menu_t menu_velocity;

extern set_t test_values; 

extern limit_t limits_menu_main;


// extern menu_t menu_main;
// extern menu_t menu_properties;

#endif
