
#define F_CPU   600000UL
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

volatile uint16_t ms = 0;
volatile uint16_t seconds = 0;
volatile uint32_t secondsUptime = 0;

uint8_t disturbWait = 0;



/**
 * Clock 1 MHz, prescaler to 1024, timer1 period of 1/(1E6/1024) = 0.001024 s. 
 * Count 198 clock cycles to amount to nearly 0.202 second (198 * 0.001024s = 0,202752s).
 * * Clock 1.2MHz Preescaler 1024. 1/1.2E6/1024 =  0,0008533s
 * Count 235 clock cycles to 0,200533333333s (235 * 0,0008533s)
 * * Clock 0.6MHz Preescaler 1024. 1/0.6E6/1024 = 0,00170666666667s
 * Count 117 clock cycles to 0,19968s (235 * 0,0008533
 * 
*/
inline void initTimer1()
{
    
  TCCR0A |= (1 << WGM01); // clear timer on compare match
  TCCR0B |= (1 << CS02) | (1 << CS00); //clock prescaler 1024
  OCR0A = 117; // compare match value 
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
        ms = 0;
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
    ms=ms;
    while(ms > 0)
    {
        _delay_ms(1);
        ms = ms - 1;
    }
}



inline void disturb()
{
    for(auto i = 0; i < 1000; ++i)
    {
            setPinBuzzer(true);
            for(auto j = 0; j < disturbWait; ++j)
            {
                _delay_ms(5);
            }
            setPinBuzzer(false);
            for(auto j = 0; j < disturbWait; ++j)
            {
                _delay_ms(5);
            }
    }
}



int main (void) 
{   
    h=21;
    m=26;
    setupGPIO();
    initTimer1();
    sei();    
//     setPinBuzzer(true);
//     _delay_ms(1000);
//     setPinBuzzer(false);
//     _delay_ms(1000);
//     enableSleep();
    
    setPinBuzzer(true);
    _delay_ms(100);
    setPinBuzzer(false);
    
    auto last = secondsUptime;
    auto lastM = m;
    while(1)
    {
        if (secondsUptime - last > 10)
        {
            setPinBuzzer(true);
            _delay_ms(50);
            setPinBuzzer(false);
            last = secondsUptime;
        }
        
        /*if(lastM != m)
        {
            setPinBuzzer(true);
            _delay_ms(100);
            setPinBuzzer(false);
            //     _delay_ms(1000);
            lastM = m;
        }*/
        
           /* if(secondsUptime - last > 5)
            {
                disturb();
                last = secondsUptime;
                disturbWait++;
                disturbWait = disturbWait % 12;
            }*/
        
//     enableSleep();
    
    }
}
