#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "encoder.h"
#include "project.h"

volatile uint8_t new_state;
volatile uint8_t old_state;
volatile uint8_t changed =0;
extern volatile uint8_t low_limit;
extern volatile uint8_t high_limit;
volatile int16_t count = 0;	
extern volatile uint8_t mode;

void encoder_init(void){
    //encoder section
    //enable the encoder pins for PC1 and PC2
    DDRC &= ~((1<<PC1) | (1<<PC2));
    //enable the pull up resistors
    PORTC |= (1<< PC1) | (1<< PC2);
    //enable the pin change interrupt for PORTC
    PCICR |= (1<<PCIE1);
    //enable interrupts for PC1 and PC2
    PCMSK1 |= (1<<PCINT9) | (1<<PCINT10);
    uint8_t x = PINC;

    // Read the A and B inputs to determine the intial state.
    // In the state number, B is the MSB and A is the LSB.
    // Warning: Do NOT read A and B separately.  You should read BOTH inputs
    // at the same time, then determine the A and B values from that value.  
	
	//given in lab
	uint8_t a = (x & (1 << PC1)) >> PC1;
	uint8_t b = (x & (1 << PC2)) >> PC2;

    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;

    new_state = old_state;
}


//taken from lab 6, adjusted for project 
ISR(PCINT1_vect)
{
	// For each state, examine the two input bits to see if state
	// has changed, and if so set "new_state" to the new state,
	// and adjust the count value.

    //  add code to read the encoder inputs and determine the new
    // count value.

	//get values of a and b, intialize
	//all and the x value with PINC, check the state and change the state as 
	//needed. also change count as states change 
	uint8_t a, b;
	uint8_t x;
	x = PINC;
	a = (x &(1<<PC1)) >>PC1;
	b= (x & (1 << PC2)) >> PC2;
    if(mode ==0){
        count = low_limit;
    }
    else{
        count = high_limit;
    }
	if (old_state == 0) {

	    // Handle A and B inputs for state 0
		if (a && !b){
			new_state=1;
			count++;
		}
		else if(!a && b){
			new_state =2;
			count--;
		}
	}
	else if (old_state == 1) {

	    // Handle A and B inputs for state 1
		if(a && b){
			new_state = 3;
			count++;
		}
		else if(!a && !b){
			new_state = 0;
			count--;
		}
	}
	else if (old_state == 2) {

	    // Handle A and B inputs for state 2
		if(!a && !b){
			new_state = 0;
			count++;
		}
		else if(a && b){
			new_state =3;
			count--;
		}
	}
	else {   
		//old_state = 3;

	    // Handle A and B inputs for state 3
		if(!a && b){
			new_state =2;
			count++;

		}
		else if(a && !b){
			new_state =1;
			count--;
		}
	}

	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
	    changed = 1;
	    old_state = new_state;

        if(mode==1){
            if(count > 99){
                count = 99;
            }
            if(count <low_limit){
                count = low_limit;
            }
            high_limit = count;
        }
        else{
            if(count < 1){ 
                count=1;
            }
            if(count > high_limit){
                count = high_limit;
            }
            low_limit = count;
        }
	}
}