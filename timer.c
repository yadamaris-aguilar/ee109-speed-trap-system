#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

volatile uint16_t tone_cycles =0;
//counter to track which tone int he alert sequence is playing (0-3)
volatile uint16_t x =0;

//timer 1 already used for the count (used in lab6) so instead use timer0
//taken from lab 6 - intialize timer0 for buzzer tone generation,, CTC mode with compare match interrupt enabled 
void timer0_init(void){
    //set CTC mode
    TCCR0A =(1<<WGM01);
    //enable the compare match interrupt
    TIMSK0 |= (1<<OCIE0A);
}
//Timer0 compare match ISR- generated the buzzer anhd manages alert signals
ISR(TIMER0_COMPA_vect)
{
    //add code to change the output bit to the buzzer, and to turn
    // off the timer after enough periods of the signal have gone by.
	//flip the bit of the buzzer
	PORTC ^= (1 << PC3);
	//decrease the toggle cycles you have 
	tone_cycles--;
	//if the tone is done playing
	if(tone_cycles == 0){
		//stop the timer
        TCCR0B &= ~((1<<CS02)|(1<<CS00));
        //go onto the next tone seuqence 
        x++;
        //alert sequence is 120-150-80-150-stop
        if(x==1){
            play_tone(150);
        }
        else if(x ==2){
            play_tone(80);
        }
        else if(x ==3){
            play_tone(150);
        }
        else{
            //reset the sequence (alert is complete)
            x=0;
        }
	}
}
//
void alert_signal(void){
    //play one of the tones
	play_tone(120);
}
//generates at one at the frequency 
void play_tone(uint16_t freq){
    //calculate the number of half cucles, (2 toggles per cycle)
    tone_cycles = 2*freq;
    //calculate the timer0 compare value for desired frequency 
    //clock is 16MHz, Prescalar is 1024, timer freq is 15625 Hz 
    OCR0A =(15625/tone_cycles/3);
    //start time 0- with prescalar of 1024 (CS02 =1, CS00=1)
    TCCR0B |= ((1<<CS02)|(1<<CS00));
}
//calculate the servo PWM cycle based on speed. Maps the servo position using airthmetic, no floating
void math_servo(uint8_t speed){
    //speed is from 0 to 100
    if (speed > 100){
        speed = 100;
    } 
    //Range: speed is from 0, OCR2A is 35, when speed is 100, OCR2A is 11
    uint16_t temp = 24 * speed;
    OCR2A = 35 - (temp/100);
}
void timer2_init(void){
    DDRB |= (1<<PB3);
    //fast PWM mode
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    //set OC2A and clear on compare match
    TCCR2A |= ( 1 << COM2A1);
    TCCR2A &= ~(1<< COM2A0);

    //set prescalar to 1024
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1<<CS20);

}
//initialzie timer1 for 4.0 second timeout in CTC mode
void timer1_init(void){
    //set the CTC mode
    TCCR1B |= (1<<WGM12);
    //enable the compare match interrupt 
    TIMSK1 |= (1<<OCIE1A);
    //set the compare value for 4.0 seconds 
    //prescalar of 1024 for 16Mhz clock, so 16MHz/1024 = 15625 
    //15625 * 4s = 62500 ticks 
    OCR1A = 62500;
}