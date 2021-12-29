/*
 * main.c
 *
 * Created: 12/21/2021 4:49:06 PM
 *  Author: Lenovo
 */ 

#define F_CPU 1000000UL

#include <stdio.h>
#include <string.h>
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>

#define UBRR_VALUE(BAUDRATE) (((F_CPU / (BAUDRATE * 8UL))) - 1)

uint8_t buffer[100];
uint16_t len;
void UART_Init(uint16_t baud);

uint16_t adcData;


void UART_Transmit(uint8_t data );

uint8_t UART_Receive( void );

void UART_SendData(uint8_t *dataPtr,uint16_t size);

void InitADC(void);

uint16_t ReadADC(uint8_t ADCchannel);

int main(void)
{
	UART_Init(9600);
	UART_Transmit('h');
	UART_Transmit('i');
	UART_Transmit('\r');
	UART_Transmit('\n');
	len = sprintf((char*)buffer,"configuracion correcta\r\n");
	
	UART_SendData(buffer,len);
	InitADC();
    while(1)
    {
        //TODO:: Please write your application code 
		adcData = ReadADC(0);
		len = sprintf((char*)buffer,"ADC->%d\r\n",adcData);
		UART_SendData(buffer,len);
		_delay_ms(500);
    }
}

void UART_Init(uint16_t baud){
	baud = UBRR_VALUE(baud);
	/*set baudrate*/
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t)baud;
	/*double speed*/
	UCSR0A = 1U<<U2X0;
	/*Enable receiver and transmitter*/
	UCSR0B |= 1U<<RXEN0 | 1U<<TXEN0;
	/*Set communication parameters*/
	UCSR0C |= 1U<<UCSZ01 | 1U<<UCSZ00;	//8-bit de datos
	UCSR0C |= 0U<<USBS0;				//1-bit stop
	UCSR0C |= 0U<<UPM01 | 0U<<UPM00;    //no parity
}

void UART_Transmit(uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

uint8_t UART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}

void UART_SendData(uint8_t *dataPtr,uint16_t size){
	while(size>0){
		UART_Transmit(*dataPtr);
		dataPtr++;
		size--;
	}
}

void InitADC(void)
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
