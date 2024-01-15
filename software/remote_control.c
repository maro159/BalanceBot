#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "pins.h"
#include "remote_control.h"

recursive_mutex_t remote_control_mutex;
static volatile float remote_target_speed = 0;

void remote_control_run()
{   
    if(!recursive_mutex_is_initialized(&remote_control_mutex))
    {
        recursive_mutex_init(&remote_control_mutex);
    } 
    // uart_puts(BT_UART, "Running!\n");
    printf("Core 1 running!\n");
    int8_t value = 0;
    while (1)
    {
        value = (int8_t)(uart_getc(BT_UART));
        // uart_putc_raw(BT_UART, (char)value);
        recursive_mutex_enter_blocking(&remote_control_mutex);
        remote_target_speed = (float)(value) / 128.0;
        printf("%f\n", remote_target_speed);
        recursive_mutex_exit(&remote_control_mutex);
    }
}

bool try_get_remote_target_speed(float *out)
{
    if(recursive_mutex_try_enter(&remote_control_mutex, NULL))
    {
        *out = remote_target_speed*6;
        recursive_mutex_exit(&remote_control_mutex);
        return true;
    }
    else
    {
        return false;
    }
}