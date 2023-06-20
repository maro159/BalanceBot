#include <stdio.h>
#include "pico/stdlib.h"
#include "encoder_rot.h"
#include "pins.h"

volatile bool status_A = false;
volatile bool status_B = false;
volatile bool status_SW = false;
int32_t last_count = 0; 
volatile int counter_en = 0;
int encoder_min = 0;
int encoder_max = 0;

void encoder_init(int min, int max) 
{
    gpio_set_irq_enabled(ENC_ROT_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENC_ROT_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENC_ROT_SW, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(&encoder_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    last_count = counter_en;
    encoder_set_limit(min, max);
   
}

void encoder_set_limit(int min, int max)
{
    encoder_min = min; 
    encoder_max = max-1; 


}

void encoder_callback(uint gpio, uint32_t event_mask)
{
    switch(gpio)
    {
        case ENC_ROT_A:
        status_A = true;
        if(status_B)
        {
            counter_en++;    
            status_B = false;
            status_A = false;     
            if(counter_en > encoder_max)
            {
                counter_en = encoder_min;
            }   
        }
        break;
        case ENC_ROT_B:
        status_B = true;
        if(status_A)
        {
            counter_en--;    
            status_B = false;
            status_A = false; 
            if(counter_en < encoder_min)
            {
                counter_en = encoder_max;
            }          
        }
        break;
        case ENC_ROT_SW:
        status_SW = true;     
        break;
        default:
        printf("encoder_callback: %u \n",gpio);
        break;
    } 
}


bool encoder_changed()
{
    if(last_count != counter_en)
    {
        // printf("enc_last_count: %d \n", counter_en);
        last_count = counter_en;
        return true;
    }
    else return false;
}




