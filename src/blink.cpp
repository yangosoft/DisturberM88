
#define F_CPU   1000000UL
#include <avr/io.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 






#define BUZZER_PIN PB3
#define BUZZER_PORT PORTB
#define BUZZER_DDR DDRB

volatile uint16_t seconds = 0;
volatile uint32_t secondsUptime = 0;

enum class STATE : int { WAIT, ACTIVE };
STATE state;


/**
 * clock runs at 1 MHz, so if we set the prescaler to 8192, the clock used for the timer will have a period of 1/(1E6/8192) = 8.192 ms. So we will need to count 122 of these adjusted clock cycles to amount to nearly 1 second (122*8.192ms = 999.424 ms).
*/
void initTimer1()
{
  TCCR1 |= (1 << CTC1);  // clear timer on compare match
  TCCR1 |= (1 << CS13) | (1 << CS12) | (1 << CS11); //clock prescaler 8192
  OCR1C = 122; // compare match value 
  TIMSK |= (1 << OCIE1A); // enable compare match interrupt
}

void setupGPIO()
{       
    BUZZER_DDR |= (1<<BUZZER_PIN);
}


void setPinBuzzer(bool on)
{
    if(on)
    {
        BUZZER_PORT |= _BV(BUZZER_PIN);
    }else
    {
        BUZZER_PORT &= ~_BV(BUZZER_PIN);
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
    setPinBuzzer(true);
    _delay_ms(1);
    setPinBuzzer(false);
    _delay_ms(1000);
    enableSleep();
    while(1)
    {
        if (seconds == 59)
        {
            setPinBuzzer(true);
            _delay_ms(1);
            setPinBuzzer(false);
            _delay_ms(50);
            seconds = 0;
            enableSleep();
        }
    }
}
