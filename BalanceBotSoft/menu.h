#ifndef _MENU_H_
#define _MENU_H_
#include "pico/stdlib.h"
#include "pico/float.h"

#define MENU_SIZE(m) (sizeof(m)/sizeof(m[0]))

typedef enum menu_type_t
{
    MENU_NORMAL,
    MENU_OPTION,
} menu_type_t;

typedef struct option_t
{
    char* name;
    void* ptr;
} option_t;

typedef struct limit_t
{
    float min;
    float max;
    float step;
    
} limit_t;

typedef struct menu_t
{
    menu_type_t menu_type;
    option_t *options;  // pointer to array
    limit_t limits;
} menu_t;

extern option_t menu_main_options[];
extern option_t menu_properties_options[];
extern option_t menu_velocity_options[];
extern option_t menu_angle_options[];

extern menu_t menu_main;
extern menu_t menu_properties;
extern menu_t menu_velocity;
extern menu_t menu_angle;

// static menu_t *current_menu; // TODO: is static ok?
// static float current_value;    // TODO: is static ok?

void menu_init();
int menu_execute();
void menu_bind_parameter(void *param, option_t *option);

#endif
