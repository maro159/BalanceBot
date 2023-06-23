#ifndef _ENCODER_ROT_H_
#define _ENCODER_ROT_H_

extern int32_t last_count; 
extern volatile int counter_en;
extern volatile bool status_SW; 
extern int encoder_min;
extern int encoder_max;

void encoder_init(int min, int max);
void encoder_set_limit(int min, int max);
void encoder_callback(uint gpio, uint32_t event_mask);
bool encoder_changed(); 




#endif