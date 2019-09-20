
#define F_CPU   1000000UL
#include <avr/io.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 


struct Time 
{
    uint16_t day;
    uint8_t h;
    uint8_t m;
    uint8_t s;
    Time(uint16_t day, uint8_t h, uint8_t m, uint8_t s)
    {
        this->day = day;
        this->h = h;
        this->m = m;
        this->s = s;
    }
    
};


Time now{260,8,0,0};

#define BUZZER_PIN PB3
#define BUZZER_PORT PORTB
#define BUZZER_DDR DDRB

volatile uint8_t halfseconds = 0;
volatile uint16_t seconds = 0;
volatile uint32_t secondsUptime = 0;

enum class STATE : int { WAIT, ACTIVE };
STATE state;


/**
 * Clock 1 MHz, prescaler to 256, timer1 period of 1/(1E6/1024) = 0.000256 s. 
 * Count 198 clock cycles to amount to nearly 0.5 second (198 * 0.000256s = 0.05688s).
*/
void initTimer1()
{
    
  TCCR0A |= (1 << WGM01); // clear timer on compare match
  TCCR0B |= (1 << CS02) | (1 << CS00); //clock prescaler 1024
  OCR0A = 198; // compare match value 
  TIMSK0 |= (1 << OCIE0A); // enable compare match interrupt
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


ISR(TIM0_COMPA_vect)
{
    halfseconds++;
    if(halfseconds == 2)
    {
        seconds++;
        secondsUptime++;
        now.s++;
        halfseconds = 0;
    }
    if ( now.s > 59)
    {
        now.s = 0;
        now.m++;
        if ( now.m > 59 )
        {
            now.m = 0;
            now.h++;
            if ( now.h > 24 )
            {
                now.h = 0;
                now.day++;
            }
        }
    }
}


/* @warning: delays in 10ms steps  */
void delayMs(int32_t ms)
{
    while(ms > 0)
    {
        _delay_ms(10);
        ms = ms - 10;
    }
}

void disturb()
{
    setPinBuzzer(true);
    _delay_ms(1);
    setPinBuzzer(false);
    _delay_ms(50);
    seconds = 0;
}


void disturb1()
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

void disturb2()
{
    uint32_t i = 500;
    int32_t j = 1000;
    while(1)
    {
        setPinBuzzer(true);
        delayMs(i);
        setPinBuzzer(false);
    
        if(j>0)
        {
                j--;
        }
        delayMs(j);
            
        i++;
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
        
        if((now.h == 1) && (now.m < 30 ))
        {
            disturb();
        }
        else
        {
            enableSleep();
        }
    }
}
