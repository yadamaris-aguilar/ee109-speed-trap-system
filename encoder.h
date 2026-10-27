#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

//global encoder variables 
//the current state of encoder
extern volatile uint8_t new_state;
//the previous state 
extern volatile uint8_t old_state;
// if the enconder changed or not (1 = yes, 0=no)
extern volatile uint8_t changed;

//function prototypes
//initializes the rotary encoder pins, the pull-ups, and the interrupts
void encoder_init(void);


#endif