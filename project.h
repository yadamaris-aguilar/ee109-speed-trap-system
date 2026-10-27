#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>

//Distance between sensors in cm 
#define DISTANCE 35

//global variables
//stores the timer count when the stop sensor is on
extern volatile int timer_count;
//means that the timing is in progress
extern volatile uint8_t timing;
//4 seconds are up 
extern volatile uint8_t timing_limit_reached;
//used to know if to print low or high mode on lcd 
extern volatile uint8_t mode;
//print new values on lcd
extern volatile uint8_t lcd_update_needed;
//most recent or new speed
extern volatile uint16_t measured_speed;
//store all three previous speeds
extern int past_speeds[3];


//function prototypes 
//calculate the speef and display them
void display_results(void);
void range_led(uint16_t speed, uint8_t low_limit, uint8_t high_limit);
//check the last three speeds and play buzzer if needed
void check_signals(uint16_t current_speed);

#endif