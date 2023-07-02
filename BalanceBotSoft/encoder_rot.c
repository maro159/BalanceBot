#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "encoder_rot.h"
#include "pins.h"

static volatile bool status_SW = false;
static volatile int32_t encoder_count = 0;
static int32_t encoder_last = 0;
static int32_t encoder_min = 0;
static int32_t encoder_max = 0;


void init_encoder() 
{
    gpio_set_irq_enabled(ENC_ROT_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENC_ROT_SW, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(&encoder_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    encoder_count = 0;
    encoder_last = 0;
    encoder_limit(0, 0);
}

void encoder_limit(int32_t min, int32_t max)
{
    encoder_min = min; 
    encoder_max = max; // TODO: -1 czy nie
}

void encoder_callback(uint gpio, uint32_t event_mask)
{
    if(gpio == ENC_ROT_SW) status_SW = true;
    else if(gpio == ENC_ROT_A)
    {   
        if(gpio_get(ENC_ROT_B)) encoder_count--;
        else encoder_count++;

        #if ENC_GO_AROUND == 0
        if(encoder_count > encoder_max) encoder_count = encoder_max;
        if(encoder_count < encoder_min) encoder_count = encoder_min;
        #else
        if(encoder_count > encoder_max) encoder_count = encoder_min;
        if(encoder_count < encoder_min) encoder_count = encoder_max;
        
        #endif
    }
}


bool encoder_changed()
{
    if(encoder_count != encoder_last)
    {
        encoder_last = encoder_count;
        return true;
    }
    else return false;
}

bool encoder_clicked()
{
    if(status_SW)
    {
        status_SW = false;
        return true;
    }
    else return false;
}

int32_t encoder_get()
{
    // int32_t count = encoder_count/4;
    // printf("%d,%d,%d,%d,%d\n", encoder_count, count, 4*count, encoder_max, encoder_last);
    return(encoder_count);
}

void encoder_set(int32_t value)
{
    encoder_count = value;  // TODO: check if within limits   
}



