#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "pins.h"
#include "remote_control.h"

recursive_mutex_t remote_control_mutex;
// static volatile float remote_target_speed = 0;
static volatile remote_targets_t remote_targets =
{
    .robot_speed = 0.0,
    .turn_speed = 0.0,
};


void remote_control_run()
{   
    if(!recursive_mutex_is_initialized(&remote_control_mutex))
    {
        recursive_mutex_init(&remote_control_mutex);
    } 
    // uart_puts(BT_UART, "Running!\n");
    int8_t data[2]= {0};
    int32_t data_index = -1;
    printf("Core 1 running!\n");
    while (1)
    {
        int8_t value = (int8_t)(uart_getc(BT_UART));
        if(value == -128) data_index = 0;
        else if(data_index >= 0)
        {
            data[data_index++] = value;
        }

        if(data_index >= 2)
        {
            recursive_mutex_enter_blocking(&remote_control_mutex);
            remote_targets.robot_speed = (float)(data[0]) / (float)(127.0);
            remote_targets.turn_speed = (float)(data[1]) / (float)(127.0);
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