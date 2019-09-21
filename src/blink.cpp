
#define F_CPU   1000000UL
#include <avr/io.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 

volatile uint8_t day{0};
volatile uint8_t h{0};
volatile uint8_t m{0};



#define BUZZER_PIN PB3
#define BUZZER_PORT PORTB
#define BUZZER_DDR DDRB

volatile uint8_t ms = 0;
volatile uint16_t seconds = 0;
volatile uint32_t secondsUptime = 0;




/**
 * Clock 1 MHz, prescaler to 1024, timer1 period of 1/(1E6/1024) = 0.001024 s. 
 * Count 198 clock cycles to amount to nearly 0.202 second (198 * 0.001024s = 0,202752s).
*/
inline void initTimer1()
{
    
  TCCR0A |= (1 << WGM01); // clear timer on compare match
  TCCR0B |= (1 << CS02) | (1 << CS00); //clock prescaler 1024
  OCR0A = 198; // compare match value 
  TIMSK0 |= (1 << OCIE0A); // enable compare match interrupt
}

inline void setupGPIO()
{       
    BUZZER_DDR |= (1<<BUZZER_PIN);
}


inline void setPinBuzzer(bool on)
{
    if(on)
    {
        BUZZER_PORT |= _BV(BUZZER_PIN);
    }else
    {
        BUZZER_PORT &= ~_BV(BUZZER_PIN);
    }
}


inline void enableSleep()
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


ISR(TIM0_COMPA_vect)
{
    ms+=200;
    if(ms >= 1000)
    {
        
        secondsUptime++;
        seconds++;
        
    }
    if ( seconds > 59)
    {
        seconds = 0;
        m++;
        if ( m > 59 )
        {
            m = 0;
            h++;
            if ( h > 24 )
            {
                h = 0;
                day++;
            }
        }
    }
}



inline void delayMs(int32_t ms)
{
    while(ms > 0)
    {
        _delay_ms(1);
        ms = ms - 1;
    }
}



inline void disturb()
{
    uint32_t i = 500;
    int32_t j = 100;
    
    auto now = secondsUptime;
    
    while(1)
    {
        setPinBuzzer(true);
        delayMs(i);
        setPinBuzzer(false);
    
        if(j>0)
        {
                delayMs(j);
                j--;
        }
            
        i++;
        if((secondsUptime - now) > 1)
        {
            now = 0;
            do
            {
                enableSleep();
                now++;
            }while(now < 5);
            now = secondsUptime;
        }
    }
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
        
        if((h == 1) && (m < 30 ))
        {
            disturb();
        }
        else
        {
            enableSleep();
        }
    }
}
