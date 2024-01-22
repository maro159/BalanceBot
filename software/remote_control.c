#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "pins.h"
#include "remote_control.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "btstack_run_loop.h"
#include "bluetooth_support/btstack_config.h"

int btstack_main(int argc, const char * argv[]);

recursive_mutex_t remote_control_mutex;
// static volatile float remote_target_speed = 0;
static volatile remote_targets_t remote_targets =
{
    .robot_speed = 0.0,
    .turn_speed = 0.0,
};

void remote_data_convert(char *data,volatile remote_targets_t *remote_values)
{
    char angle_array[4] = {0}; 
    char radius_array[5] = {0}; 

    float radius_value  = 0.0f; 
    float angle_value_degrees = 0.0f;
    float angle_value_rad = 0.0f; 
    
    float robot_speed_calculated = 0.0f; 
    float turn_speed_calculated = 0.0f;

    strncpy(angle_array, data, 3);
    strncpy(radius_array, (data + 3), 4);
    
    radius_value = atof(radius_array);
    angle_value_degrees = atof(angle_array);

    angle_value_rad = (angle_value_degrees / 360.0f) * 2.0f * (float)M_PI;
    
    robot_speed_calculated = radius_value * sinf(angle_value_rad) / 1000.0f;
    turn_speed_calculated = radius_value * cosf(angle_value_rad) / 1000.0f;
	
    PRINT("speed:%f\n", robot_speed_calculated);
    PRINT("turn:%f\n", turn_speed_calculated);

    remote_values->robot_speed = robot_speed_calculated;
    remote_values->turn_speed = turn_speed_calculated;   
}


void remote_control_run()
{   
    if(!recursive_mutex_is_initialized(&remote_control_mutex))
    {
        recursive_mutex_init(&remote_control_mutex);
    } 
    // uart_puts(BT_UART, "Running!\n");
    char data[7]= {0};
    int32_t data_index = -1;
    printf("Core 1 running!\n");
    while (1)
    {
        char value = (char)(uart_getc(BT_UART));
        if(value == '#') { data_index = 0; }
        else if(data_index >= 0)
        {
            data[data_index++] = value;
        }
       
        if(data_index >= 7)
        {
            PRINT("Wait for mutex\n");
            recursive_mutex_enter_blocking(&remote_control_mutex);
            remote_data_convert(data, &remote_targets);
            // remote_targets.robot_speed = (float)(data[0]) / (float)(127.0);
            // remote_targets.turn_speed = (float)(data[1]) / (float)(127.0);
            recursive_mutex_exit(&remote_control_mutex);
            data_index = -1;
        }
    }
}

void remote_control_run_bt_internal()
{   
    if (cyw43_arch_init()) {
        printf("cyw43_arch_init() failed.\n");
        return;
    }
    char data[7]= {0};
    int32_t data_index = -1;
    printf("Core 1 running!\n");

    btstack_main(0, NULL);
    btstack_run_loop_execute();

    while (1)
    {
        char value = (char)(uart_getc(BT_UART));
        if(value == '#') { data_index = 0; }
        else if(data_index >= 0)
        {
            data[data_index++] = value;
        }
       
        if(data_index >= 7)
        {
            PRINT("Wait for mutex\n");
            recursive_mutex_enter_blocking(&remote_control_mutex);
            remote_data_convert(data, &remote_targets);
            // remote_targets.robot_speed = (float)(data[0]) / (float)(127.0);
            // remote_targets.turn_speed = (float)(data[1]) / (float)(127.0);
            recursive_mutex_exit(&remote_control_mutex);
            data_index = -1;
        }
    }
}

// bool try_get_remote_target_speed(float *out)
// {
//     if(recursive_mutex_try_enter(&remote_control_mutex, NULL))
//     {
//         *out = remote_target_speed*6;
//         recursive_mutex_exit(&remote_control_mutex);
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }

bool try_get_remote_data(remote_targets_t *data_frame)
{
    if(recursive_mutex_try_enter(&remote_control_mutex, NULL))
    {
        *data_frame = remote_targets;
        recursive_mutex_exit(&remote_control_mutex);
        return true;
    }
    else
    {
        return false;
    }
}