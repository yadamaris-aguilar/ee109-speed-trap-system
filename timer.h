#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>


//global timer variables 
//lab 6: number of buzzer toggles remaining
extern volatile uint16_t tone_cycles;

//buzzer with timer 0
void timer0_init(void);
void play_tone(uint16_t freq);
//tone played if the 3 speeds are higher than the high_limit or lower than the low_limit
void alert_signal(void);

//timer 1
void timer1_init(void);

//servo with timer 2
//initialzie timer2 for the servo pwm control
void timer2_init(void);
//calculate and set the servo position based on the speed
void math_servo(uint8_t speed);

#endif