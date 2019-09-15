
#define F_CPU   1000000UL
#include <avr/io.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 


#define LED _BV(PD6)     // = (1 << PB0)


#define LED_DDR DDRD
#define LED_PORT PORTD



void setupGPIO()
{
       
    DDRB |= (1<<PB3);
       
}


void setGreen(bool on)
{
    if(on)
    {
        PORTB |= _BV(PB3);
    }else
    {
        PORTB &= ~_BV(PB3);
    }
}





int main (void) 
{   
    setupGPIO();
    while(1)
    {
        setGreen(true);
        _delay_ms(25);
        setGreen(false);
        _delay_ms(25);
        
    }
}
