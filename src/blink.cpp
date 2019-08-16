/*ORIGINAL E:F8, H:DC, L:E2
 * 1MHZ E:F8, H:DD, L:62
 128kHz -U lfuse:w:0xe3:m -U hfuse:w:0xdd:m -U efuse:w:0xf8:m
 */

#define F_CPU   1000000UL
#include <avr/io.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 


#define LED _BV(PD6)     // = (1 << PB0)


#define LED_DDR DDRD
#define LED_PORT PORTD

volatile int disable = 0;


enum class STATE : int { SLEEP, NORMAL, BLINK, GROW, FIXED_RED, FIXED_GREEN, FIXED_YELLOW };

STATE state;
int counter = 0;
static constexpr int REPEAT_SEQUENCE_BEFORE_SLEEP = 4;

/*
 5v--- BTN-----> PIN4 (INT0)
        |
        150R
        |
        GND
        
 GND---150R---|<RED     ----> PIN11 (PD6)
 GND---150R---|<YELLOW  ----> PIN12 (PD7)
 GND---150R---|<GREEN   ----> PIN17 (PB0)

*/


void setupGPIO()
{
    /*DDRB = 0xFF; //1 output port
    PORTB = 0xFF; // 1 high
    
    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);
    
    
    DDRB  = 0b00001000;   // PB3 output
    DDRD  = 0b01100000;   // PD5 and PD6 outputs
    
    
    
    DDRD |= (1<<PD7);
    
    DDRB |= (1<<PB0);*/
    
    DDRC |= (1<<PC0);
       
}


void setupPWM()
{
     DDRB |= (1 << DDB3);
    // PB3 is now an output

    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);      // Non inverting mode on OC0A and OC0B, Mode = Mode 3 FAST PWM
    TCCR0B = _BV(CS00);                                                // No prescaling
 
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);      // Non inverting mode on OC2A, Mode = Mode 3 FAST PWM
    TCCR2B = _BV(CS20) | _BV(CS21) | _BV(CS22);  
    
    OCR2A = 128;
}

void setRed(bool on)
{
    if(on)
    {
        PORTC |= _BV(PC0);
    }else
    {
        PORTC &= ~_BV(PC0);
    }
}

void setYellow(bool on)
{
    if(on)
    {
        PORTD |= _BV(PD7);
    }else
    {
        PORTD &= ~_BV(PD7);
    }
}

void setGreen(bool on)
{
    if(on)
    {
        PORTB |= _BV(PB0);
    }else
    {
        PORTB &= ~_BV(PB0);
    }
}



ISR (INT0_vect)
{
      EIMSK = 0;
      setRed(false);
      setYellow(false);
      setGreen(false);
    
      if(state == STATE::BLINK)
      {
          state = STATE::GROW;
      }else if(state == STATE::NORMAL)
      {
          state = STATE::FIXED_GREEN;
      }
      else if(state == STATE::FIXED_GREEN)
      {
          state = STATE::FIXED_YELLOW;
      }
      else if(state == STATE::FIXED_YELLOW)
      {
          state = STATE::FIXED_RED;
      }
      else if(state == STATE::FIXED_RED)
      {
          state = STATE::BLINK;
      }
      else if(state == STATE::GROW)
      {
          state = STATE::NORMAL;
      }
      _delay_ms(500);
}



void disableInt0()
{
     cli();
     PCMSK0 &= ~(1 << PCINT0); 
     EIMSK &= ~(1 << INT0);
}

void enableSleep()
{
    setRed(true);
    setYellow(true);
    setGreen(true);
    _delay_ms(2000);
    setRed(false);
    setYellow(false);
    setGreen(false);
    
    
    cli();
    PCMSK0 |= (1 << PCINT0); 
    EIMSK |= (1 << INT0);
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    
    sleep_enable();
    cli();
    sei();
    // actually sleep
    sleep_cpu();
    sleep_disable();
        
}






int main (void) 
{   
    setupGPIO();
    setupPWM();
    uint8_t i = 0;
    while(1)
    {
        setRed(true);
        TCCR2B &= ~(1<<CS22);
        _delay_ms(2000);
        setRed(false);
         TCCR2B = _BV(CS20) | _BV(CS21) | _BV(CS22); 
        _delay_ms(2000);
        ++i;
        if ( i == 3 )
        {
            PORTB &= ~_BV(PB3);
            enableSleep();
        }
    }
   
    
}
