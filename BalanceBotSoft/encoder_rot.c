#include <stdio.h>
#include "pico/stdlib.h"
#include "encoder_rot.h"
#include "pins.h"

volatile bool status_A = false;
volatile bool status_B = false;
volatile bool status_SW = false;
int32_t encoder_last = 0; 
volatile int encoder_count = 0;
int encoder_min = 0;
int encoder_max = 0;

void encoder_init() 
{
    gpio_set_irq_enabled(ENC_ROT_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENC_ROT_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENC_ROT_SW, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(&encoder_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    encoder_last = encoder_count;
    encoder_set_limit(0, 0);
}

void encoder_set_limit(int min, int max)
{
    encoder_min = min; 
    encoder_max = max; // TODO: -1 czy nie
}

void encoder_callback(uint gpio, uint32_t event_mask)
{
    switch(gpio)
    {
        case ENC_ROT_A:
        status_A = true;
        if(status_B)
        {
            encoder_count++;    
            status_B = false;
            status_A = false;     
            if(encoder_count > encoder_max)
            {
                encoder_count = encoder_min;
            }   
        }
        break;
        case ENC_ROT_B:
        status_B = true;
        if(status_A)
        {
            encoder_count--;    
            status_B = false;
            status_A = false; 
            if(encoder_count < encoder_min)
            {
                encoder_count = encoder_max;
            }          
        }
        break;
        case ENC_ROT_SW:
        status_SW = true;     
        break;
        default:
        printf("encoder_callback: %u \n",gpio); // TODO: usunąć
        break;
    } 
}


bool encoder_changed()
{
    if(encoder_last != encoder_count)
    {
        // printf("enc_last_count: %d \n", counter_en);
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

int encoder_get()
{
    return encoder_count;
}

void encoder_set(int value)
{
    encoder_count = value;
}



