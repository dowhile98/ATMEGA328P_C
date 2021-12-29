/*
 * main.c
 *
 * Created: 12/19/2021 8:20:29 PM
 */  

#define F_CPU 8000000UL

#include <stdio.h>
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define VREF 5

uint8_t duty;


int USART0SendByte(char u8Data, FILE *stream);
void USART0Init(void);
//set stream pointer
FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, NULL, _FDEV_SETUP_WRITE);
/************************************************************************/
/* change to interrupt vecto position                                   */
/************************************************************************/
void Move_interrupts(void);

void ExternalIT_Config(void);

void Timer0Config_IT(void);

void Timer0Config_FastPWM3(void);

void Timer0Config_FastPWM7(void);

int main(void)
{
	ExternalIT_Config();
	// Configure PB5 as an output using the Port B Data Direction Register
	DDRB |=  1U<<DDB4 | 1U<<DDB2;
	//Timer0Config_IT();
	Timer0Config_FastPWM7();
	sei();
	
	
    while(1)
    {
        //TODO:: Please write your application code 
		OCR0B = duty;
		duty += 2;
		if(duty>30)
		duty = 1;
		_delay_ms(100);
		
    }
}

void Move_interrupts(void)
{
	/* Enable change of Interrupt Vectors */
	MCUCR = (1<<IVCE);
	/* Move interrupts to Boot Flash section */
	MCUCR = (1<<IVSEL);
}

void ExternalIT_Config(void){
	 // Configure PD2 as an input using the Data Direction Register D (DDRD)
	 DDRD &= ~_BV(DDD2);
	 // Enable the pull-up resistor on PD2 using the Port D
	 PORTD |= _BV(PORTD2);
	
	EICRA |= 1U<<ISC01;			//falling edge sensitivity (INT0)
	EIMSK |= 1U<<INT0;			//mask INT0 
	
	return;
}

ISR(INT0_vect){
	PORTB ^= _BV(5);
}

void Timer0Config_IT(void){
	/*select clock sources and prescaler*/
	TCCR0B |= 1U<<CS02 | 1U<<CS00;			//N = 1024
	/*select mode*/
	TCCR0A |= 1U<<WGM01;
	/*Set value to compare register*/
	OCR0A = 39-1;
	/*Interrupt config*/
	TIMSK0 |= 1U<<OCIE0A;
	TIFR0 |= 1U<<OCF0A;
}

ISR(TIMER0_COMPA_vect){
	PORTB ^= 1U<<PORTB2;
}

void Timer0Config_FastPWM3(void){
	DDRD |= _BV(DDD6);
	/*se clock sources*/
	TCCR0B |= 1U<<CS02;	//N = 256
	/*select mode*/
	TCCR0A |= 1U<<WGM01 | 1U<<WGM00;		//fast PWM (mode 3)
	TCCR0A |= 1U<<COM0A1;					//Clear OC0A on Compare Match, set OC0A at BOTTOM
	/*set duty*/
	OCR0A  = 1;
}

void Timer0Config_FastPWM7(void){
	DDRD |= _BV(DDD5);
	/*se clock sources*/
	TCCR0B |= 1U<<CS02;	//N = 256
	/*select mode*/
	TCCR0A |= 1U<<WGM01 | 1U<<WGM00;		//fast PWM (mode 7)
	TCCR0B |= 1U<<WGM02;
	TCCR0A |= 1U<<COM0B1;					//Clear OC0A on Compare Match, set OC0A at BOTTOM
	OCR0A = 30;
	/*set duty*/
	OCR0B = 1;
}

void InitADC()
{
	// Select Vref=AVcc
	ADMUX |= (1<<REFS0);
	//set prescaller to 128 and enable ADC
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
}
uint16_t ReadADC(uint8_t ADCchannel)
{
	//select ADC channel with safety mask
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	//single conversion mode
	ADCSRA |= (1<<ADSC);
	// wait until ADC conversion is complete
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}

void USART0Init(void)
{
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	//enable transmission and reception
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}
int USART0SendByte(char u8Data, FILE *stream)
{
	if(u8Data == '\n')
	{
		USART0SendByte('\r', stream);
	}
	//wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = u8Data;
	return 0;
}