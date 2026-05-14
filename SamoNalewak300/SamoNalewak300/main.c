/*
 * main.c
 *
 * Created: 5/13/2026 1:08:57 PM
 *  Author: battl
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>


//Mapping pins PORTB
#define PWM PB1 //OC1A
#define COIL PB5 
#define P1 PB4
#define P2 PB3
#define P3 PB2
#define P8 PB0

//Mapping pins PORTD
#define FLOW PD4
#define P4 PD3 
#define P5 PD5
#define P6 PD6
#define P7 PD7

volatile uint16_t pulseCount = 0;
volatile uint8_t  prevState  = 0;

void clockchange(void);
void pwmInit(void);
void servo(uint8_t degrees);
void pumpOn(void);
void pumpOff(void);
void flowInit(void);
float getLiters(void);
void  toPourAlcohol(float milliliters);
void start(uint8_t count, float milliliters);
int main(void)
{
	//button
	DDRD&=~(1<<PD2);
	PORTD|=(1<<PD2);
	//button
	DDRB|=(1<<COIL);
	clockchange();
	pwmInit();
	flowInit();
	
    while(1)
    {
		if(!(PIND&(1<<PD2))){
			start(1,100.0f);
		}
    }
}

void clockchange(void)
{
	cli();
	CLKPR = 0x80;
	CLKPR= 0x00;
}
void pwmInit(void)
{
	DDRB |=(1<<PWM);
	TCCR1A=(1<<COM1A1)|(1<<WGM11);
	TCCR1B=(1<<WGM13)|(1<<WGM12)|(1<<CS11);
	ICR1 = 19999; // ocr1a =500 0 radians 2500 180 radians
	OCR1A = 1500;
}
void servo(uint8_t degrees)
{
	OCR1A = 500 + ((uint32_t)degrees * 2000) / 180;
}
void pumpOn(void)
{
	PORTB|=(1<<COIL);
}
void pumpOff(void)
{
	PORTB&=~(1<<COIL);
}
void flowInit(void)
{
	DDRD &=~(1<<FLOW);
	PORTD |= (1<<FLOW);
	prevState = (PIND >> FLOW) & 0x01;  // stan startowy
	PCICR |= (1<<PCIE2);
	PCMSK2|=(1<<PCINT20);
	sei();
}
float getLiters(void)
{
	uint16_t count;
	cli();
	count = pulseCount;
	sei();
	return (float)count / 5142.0f;
}
void toPourAlcohol(float milliliters)
{
	 float target = milliliters / 1000.0f; 
	 
	 cli();
	 pulseCount = 0;   // Reset before taking a new measurement!
	 sei();
	 
	 pumpOn();
	 while (getLiters() < target)
	 {
		 _delay_ms(10);   // wait until the appropriate amount has flowed through
	 }
	 pumpOff();
	 _delay_ms(200);
}
void start(uint8_t count , float mililiters)
{
	for (uint8_t i =0 ; i<count;i++)
	{
		servo(i*(180/(count-1)));
		_delay_ms(600);
		if (i==0)
		{
			toPourAlcohol(mililiters+10.0f); //taking the size of the pipes into account the first time 
		}
		else{
			toPourAlcohol(mililiters);
		}
	}
	servo(90);
}

ISR(PCINT2_vect)
{
	 uint8_t current = (PIND >> FLOW) & 0x01;

	 if (current == 1 && prevState == 0)  
	 {
		 _delay_us(100); 
		 if ((PIND >> FLOW) & 0x01)        
		 pulseCount++;
	 }
	 prevState = current;
}