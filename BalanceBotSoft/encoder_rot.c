#include <stdio.h>
#include "pico/stdlib.h"
#include "encoder_rot.h"
#include "pins.h"

volatile bool status_SW = false;
volatile uint32_t last_state = 0xFFFFFFFF;
volatile int32_t encoder_count = 0;  // multiplied by 4
int32_t encoder_last = 0;  // multiplied by 4
int32_t encoder_min = 0; // multiplied by 4
int32_t encoder_max = 0; // multiplied by 4


void encoder_init() 
{
    gpio_set_irq_enabled(ENC_ROT_A, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(ENC_ROT_B, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(ENC_ROT_SW, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(&encoder_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    last_state = 0xFF;   // TODO: reduntant lines
    encoder_count = 0;
    encoder_last = 0;
    encoder_set_limit(0, 0);
}

void encoder_set_limit(int min, int max)
{
    encoder_min = 4*min; 
    encoder_max = 4*max; // TODO: -1 czy nie
}

void encoder_callback(uint gpio, uint32_t event_mask)
{
    if(gpio == ENC_ROT_SW) status_SW = true;
    else if(gpio == ENC_ROT_A || gpio == ENC_ROT_B)
    {   
        // store levels of encoder inputs in one variable
        uint32_t current_state = (gpio_get(ENC_ROT_B) << 1) | (gpio_get(ENC_ROT_A));
        // combine current and previous encoder states
        uint32_t states = (last_state << 2) | (current_state);
        //  increment or decrement encoder counter depending on state change
        if(states == 0b1101 || states == 0b0100 || states == 0b0010 || states == 0b1011) encoder_count++;
        else if(states == 0b1110 || states == 0b0111 || states == 0b0001 || states == 0b1000) encoder_count--;
        last_state = current_state;

        #ifdef ENC_GO_AROUND
        if(encoder_count > encoder_max) encoder_count = encoder_min;
        if(encoder_count < encoder_min) encoder_count = encoder_max;
        #else
        if(encoder_count > encoder_max) encoder_count = encoder_max;
        if(encoder_count < encoder_min) encoder_count = encoder_min;
        #endif
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
    return encoder_count/4;
}

void encoder_set(int value)
{
    encoder_count = value;  // TODO: check limits
}



