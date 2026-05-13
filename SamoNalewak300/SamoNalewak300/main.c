/*
 * main.c
 *
 * Created: 5/13/2026 1:08:57 PM
 *  Author: battl
 */ 

#include <util/delay.h>
#include <avr/io.h>


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


void clockchange(void);
void pwmInit(void);
void servo(uint8_t degrees);

int main(void)
{
	clockchange();
	pwmInit();
	
    while(1)
    {
		servo(180);
    }
}

void clockchange(void)
{
	CLKPR = 0x80;
	CLKPR= 0x00;
	
}
void pwmInit(void)
{
	DDRB |=(1<<PWM);
	TCCR1A=(1<<COM1A1)|(1<<WGM11);
	TCCR1B=(1<<WGM13)|(1<<WGM12)|(1<<CS11);
	ICR1 = 19999; // ocr1a =1000 0 radians 2000 180 radians
	OCR1A = 1500;
}
void servo(uint8_t degrees)
{
	OCR1A = 1000 + ((uint32_t)degrees * 1000) / 180;
	
}