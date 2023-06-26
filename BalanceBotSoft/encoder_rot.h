#ifndef _ENCODER_ROT_H_
#define _ENCODER_ROT_H_

#define ENC_GO_AROUND 0 // determine if encoder_value wraps when above limits

// extern int32_t encoder_last; 
// extern volatile int encoder_count;
// extern volatile bool status_SW; 
// extern int32_t encoder_min;
// extern int32_t encoder_max;

void init_encoder();
void encoder_limit(int32_t min, int32_t max);
void encoder_callback(uint gpio, uint32_t event_mask);
bool encoder_changed(); 
bool encoder_clicked();
int32_t encoder_get();
void encoder_set(int32_t value);



#endif