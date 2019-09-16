
#define F_CPU   1000000UL
#include <avr/io.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 


#define LED _BV(PD6)     // = (1 << PB0)


#define LED_DDR DDRD
#define LED_PORT PORTD

volatile uint16_t seconds = 0;
volatile uint32_t secondsUptime = 0;

enum class STATE : int { WAIT, ACTIVE };
STATE state;


/**
 * clock runs at 1 MHz, so if we set the prescaler to 8192, the clock used for the timer will have a period of 1/(1E6/8192) = 8.192 ms. So we will need to count 122 of these adjusted clock cycles to amount to nearly 1 second (122*8.192ms = 999.424 ms).
*/
void initTimer1(void)
{
    
  TCCR1 |= (1 << CTC1);  // clear timer on compare match
  TCCR1 |= (1 << CS13) | (1 << CS12) | (1 << CS11); //clock prescaler 8192
  OCR1C = 122; // compare match value 
  TIMSK |= (1 << OCIE1A); // enable compare match interrupt
}

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


void enableSleep()
{
    cli();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    cli();
    sei();
    // actually sleep
    sleep_cpu();
    sleep_disable();
}


ISR(TIMER1_COMPA_vect)
{
    seconds++;
    secondsUptime++;
}



int main (void) 
{   
    setupGPIO();
    initTimer1();
    sei();    
    setGreen(true);
    _delay_ms(1);
    setGreen(false);
    _delay_ms(1000);
    enableSleep();
    while(1)
    {
        if (seconds == 59)
        {
            setGreen(true);
            _delay_ms(1);
            setGreen(false);
            _delay_ms(50);
            seconds = 0;
            enableSleep();
        }
    }
}
