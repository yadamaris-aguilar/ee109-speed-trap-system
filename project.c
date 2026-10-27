/********************************************
 *
 *  Name: Yadamaris Aguilar
 *  Email: yiaguila@usc.edu
 *  Section: Wednesday 2pm
 *  Assignment: Project
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "lcd.h"
#include "project.h"
#include "encoder.h"
#include "timer.h"

//timer1 count once it stops 
volatile int timer_count = 0;
volatile uint8_t timing = 0;
volatile uint8_t timing_limit_reached = 0;
//checkpoint 2 : global variable to track which limit being adjusted 
volatile uint8_t mode= 0;
//used to know if to print low or high mode on lcd 
uint8_t low_limit;
uint8_t high_limit;
volatile unsigned int measured_speed =0;
//set speeds to 0, update once we calculate actual speeds
int past_speeds[3] = {0};
//track the number of buzzers that happened, to play the alert signal if needed
volatile int count_buzzer =0;



int main(void)
{
    lcd_writecommand(1);
    //PD2/PD3 initialize as inputs for start/stop sensors
    DDRD &= ~((1<<PD2) | (1<<PD3));
    //enable the pull up resistors
    PORTD |= ((1<<PD2) | (1<<PD3));
    //PD1, yellow LED, as output
    DDRD |= (1<<PD1);

    //enable the pin change interrupt
    //use pin change map register 
    PCICR |= (1<<PCIE2);
    PCMSK2 |= (1<<PCINT18) | (1<<PCINT19);

    //checkpoint2 pin change interrupt for buttons
    //set pb4 and pb5 as inputs
    DDRB &= ~((1<<PB4) | (1<< PB5));
    //enable pull up resistors
    PORTB |= (1<< PB4) | (1<< PB5);
    //enable pin change interrupt
    PCICR |= (1<< PCIE0);
    //enable interrupts for PB4 and PB5
    PCMSK0 |= (1<< PCINT4) | (1<<PCINT5);

    //buzzer section
    //similar to lab 6, use PC3 for the buzzer output
    //set the buzzer pin as output
    DDRC |= (1<< PC3);
    //start on low
    PORTC &= ~(1<<PC3);

    //other LEDS
    //initialize the LED pins as outputs for red and for blue
    DDRC |= (1<<PC4) | (1<<PC5);
    //both start off on 0 (off)
    PORTC |= (1<<PC4) |(1<<PC5);


    //enable the encoder
    encoder_init();

    //enable the global interrupt 
    sei();

    // Initialize the LCD
    lcd_init();

    // Write splash screen and delay for 1 second
    lcd_writecommand(1);
    lcd_moveto(0,0);
	//print out my name that fits on the lcd
	lcd_stringout("Yadamaris Aguila");

    _delay_ms(1000);
    lcd_writecommand(1);

  
    //read stored values from EEPROM. From address 1 and 99
    low_limit = eeprom_read_byte((void *)1);
    high_limit = eeprom_read_byte((void *)99);

    //validate and correct EEPROM values if out of range 

    //low limit is at least 1
    if(low_limit < 1){ 
        low_limit=1;
    }
    //low limit can not go over high 
    if(low_limit > high_limit){
        low_limit = high_limit;
    }
    //high limit is 99, if over reset to 0
    if(high_limit > 99){
        high_limit = 00;
    }
    if(low_limit > high_limit){
        high_limit = low_limit;
    }

    display_results();
    //initialize other timers used
    timer0_init();
    timer1_init();
    timer2_init();


    while (1) {                 // Loop forever

        display_results();

        eeprom_update_byte((void *) 99, high_limit);
        eeprom_update_byte((void *) 1, low_limit);

    }

    return 0;   /* never reached */
}


//The sensing of the start and stop inputs, 
//turning TIMER1 on and off, and retrieving the count value 
//can all be done in the Pin Change Interrupt ISR for the two sensors.

//ISR for the sensors at PD2 and PD3
ISR(PCINT2_vect){
    //check which pin changed
    //if the timing isnt active anymore and start sensor is active
    if(!(PIND & (1<<PD2)) && !timing){
        //timer count register is cleared to 0
        TCNT1 =0;
        timer_count = 0;
        //4.0s compare value 
        OCR1A = 62500;

        //WGM12 =1 will put the timer in CTC mode
        TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);


        //turn on the Yellow LED (PD1) to indicate timing in progress
        PORTD |= (1<<PD1);

        //mark that the timing has started
        timing =1;
        //mark that the limit is 0 since timing is barely starting
        timing_limit_reached =0;
    }
    //stop sensor is active and timing is going
    if(!(PIND & (1<<PD3)) && timing){
        //we have to retrieve the count value
        //calculate the speed 
        //convert the ticks to milliseconds with (TCNT1*8)/125. 
        //Prescalar of 1024, with a clock of 16MHz then tick is 64microseconds 
        timer_count= (TCNT1*8) / 125;
        measured_speed = 3810/timer_count;

        //stop the timer by clearing the prescalar
        TCCR1B &= ~((1<<WGM12) | (1<<CS12) | (1<<CS10));
        math_servo(measured_speed);
        //controls the RGB LED based on the speed and limits 
        range_led(measured_speed, low_limit, high_limit);
        //tracks last three measuredments
        check_signals(measured_speed);
        //turn off the yellow LED to show timing is not in progress
        PORTD &= ~(1<<PD1);
        //mark the timing has stopped
        timing =0; 
    }

}

//The TIMER1 prescalar should be selected that allowed the timer to count for
//over 4 seconds before reaching its max 16-bit value. Set a timeout threshold
//to terminiate if stop never arrives

ISR(TIMER1_COMPA_vect){
    //first stop the timer
    TCCR1B &= ~ (1<<WGM12) | (1<<CS12) | (1<<CS10);
    //turn the yellow LED off to show timing not in progress
    PORTD &= ~(1<< PD1);
    //indicate timing is off 
    timing =0;
    //indicate that the timing limit is reached at 4sec
    timing_limit_reached = 1;
}

void display_results(void){
    
    if(timing_limit_reached){
        //print the message out
        lcd_moveto(0,0);
        lcd_stringout("Timing expired");
    }
    else{
        lcd_moveto(0,0);
        char buffer[17];
        //print the speed out
        sprintf(buffer, "%4dms %3dcm/s", timer_count, measured_speed);
        lcd_stringout(buffer);
        
    }

    //print out the high and low limit
    char buf[17];
    lcd_moveto(1, 0);
    //check which mode you are in so you can indicate with an * next to the H/L
    if(mode == 0){
        sprintf(buf, " H: %2d *L: %2d", high_limit, low_limit);
    }

    else{
        sprintf(buf, "*H: %2d  L: %2d", high_limit, low_limit);
    }
    lcd_stringout(buf);
}
//isr to update the mode when a button is pressed
ISR(PCINT0_vect){
    if(!(PINB &(1<<PB4))){
        //high mode
        mode =1;
    }
    else if(!(PINB & (1<< PB5))){
        //low mode
        mode = 0;
    }
}

//conteols the RBG LED based on speeds and limits
//RED LED is on whn speed is greater than high imit
//Blue is on when speed if lower than low limit
//green is on when speed is within the range
void range_led(uint16_t speed, uint8_t low_limit, uint8_t high_limit){
    //there is no measurement so green is on and red is off and blue is off
    if(speed ==0){
    PORTC |= (1<< PC4); // red is off
    PORTC |= (1<< PC5); // blue is off
    }
    //if the speed is higher than high limit, red is lit
    else if(speed>high_limit){
        PORTC &= ~(1<<PC4);
        PORTC |= (1<< PC5);
    }
    //if the speed is lower than the low limit, the blue is on
    else if(speed < low_limit){
        PORTC |= (1<< PC4);
        PORTC &= ~(1<<PC5);
    }
    //if speed is equal or higher than low and equal to or lower than high, green is on 
    else{
        PORTC |= (1<<PC4);
        PORTC |= (1<<PC5);
    }

}


//track the last three speed measurements, and then check if within range
void check_signals(uint16_t current_speed){
    //update the speeds by taking in new speeds and putting old ones farther away
    //oldest <- middle <- newest
    past_speeds[0] = past_speeds[1];
    past_speeds[1] = past_speeds[2];

    //mark the current speed as out of range (1)
    if(current_speed > high_limit || current_speed < low_limit)
    {
        //out of range
        past_speeds[2] = 1;
    }

    //check if the last three speed limits were out of range 
    if(past_speeds[0] == 1 && past_speeds[1] == 1 && past_speeds[2] == 1){
        alert_signal();
    }
}

