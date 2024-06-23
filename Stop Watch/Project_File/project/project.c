/*
 * project.c
 *
 *  Created on: Feb 3, 2024
 *      Author: Abdelrahman
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char seconds_flag = 0;
unsigned char tick = 0;
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

void timer1_inti(unsigned short Tick)
{
	TCNT1 = 0; // timer initial value.
	OCR1A = Tick; // compare value.

	TIMSK |= (1<<OCIE1A); //Enable compare interrupt
	TCCR1A = (1<<FOC1A) | (1<<FOC1B); // non PWM mode
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); //set prescaler to 1024 and  CTC mode.
}

void INT0_init(void)
{
	MCUCR &= ~(1<<ISC00);
	MCUCR |= (1<<ISC01);
	GICR  |= (1<<INT0);//Enable external interrupt 0
}

void INT1_init(void)
{
	MCUCR |= (1<<ISC10) | (1<<ISC11);
	GICR  |= (1<<INT1); //Enable external interrupt 1
}

void INT2_init(void)
{
	MCUCSR &= !(1<<ISC2);
	GICR   |= (1<<INT2);//Enable external interrupt 2
}

int main()
{
	DDRD  &= ~(1<<PD2); // set PD2 as input pin.
	PORTD |= (1<<PD2); // enable internal pull up resistor at PD2.
	DDRD  &= ~(1<<PD3); // set PD3 as input pin.

	DDRB   &= ~(1<<PB2); //set PB2 as input pin.
    PORTB |= (1<<PB2);

    DDRA |= 0x3F;

	DDRC |= 0x0F;

	PORTA |= 0x3F;
	PORTC &= 0xF0;

	SREG  |= (1<<7);

	timer1_inti(1000);
	INT0_init();
	INT1_init();
	INT2_init();
    while(1)
    {
    	if(seconds_flag == 1)
    	{
    		seconds++;

    		if(seconds == 60)
    		{
    			seconds = 0;
    			minutes++;
    		}
    		if(minutes == 60)
    		{
    			minutes = 0;
    			hours++;
    		}
    		if(hours == 24)
    		{
    			hours = 0;
    		}
    	    seconds_flag = 0;// reset the flag again
    	}
    	else
    	{
    		PORTA = (PORTA & 0xC0) | 0x01;
    		PORTC = (PORTC & 0xF0) | (seconds%10);

    		// small delay to see changes in 7 segments
    		_delay_ms(5);

    		PORTA = (PORTA & 0xC0) | 0x02;
    		PORTC = (PORTC & 0xF0) | (seconds/10);

    		_delay_ms(5);

    		PORTA = (PORTA & 0xC0) | 0x04;
    		PORTC = (PORTC & 0xF0) | (minutes%10);

    		_delay_ms(5);

    		PORTA = (PORTA & 0xC0) | 0x08;
    	    PORTC = (PORTC & 0xF0) | (minutes/10);

    	    _delay_ms(5);

    	    PORTA = (PORTA & 0xC0) | 0x10;
    	    PORTC = (PORTC & 0xF0) | (hours%10);

    	    _delay_ms(5);

    	    PORTA = (PORTA & 0xC0) | 0x20;
    	    PORTC = (PORTC & 0xF0) | (hours/10);

    	    _delay_ms(5);
    	}
    }
}



ISR(TIMER1_COMPA_vect)
{
	seconds_flag = 1;
}
//Reset interrupt
ISR(INT0_vect)
{
	seconds = 0;
	minutes = 0;
	hours = 0;
}
//Timer pause
ISR(INT1_vect)
{
	TCCR1B &= 0xF8; //clear timer bits
}
//Timer resume
ISR(INT2_vect)
{
	TCCR1B |= (1<<CS10) | (1<<CS12);// resume the timer by enable the bits again
}
