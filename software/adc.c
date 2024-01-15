#include "pico/stdlib.h"
#include "adc.h"
#include "pins.h"

void init_adc()
{
    adc_init();
    adc_gpio_init(BATTERY_LVL);
    adc_select_input(BATTERY_LVL_ADC);
}

void adc_print()
{
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t result = adc_read();
    printf("Voltage: %f V\n", result * conversion_factor);

}