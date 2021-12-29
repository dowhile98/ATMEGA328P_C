/*
 * File:   main.c
 * Author: Lenovo
 *
 * Created on 23 de julio de 2021, 05:25 PM
 */

#define F_CPU   16000000U

#include <xc.h>
#include <util/delay.h>
#include <stdio.h>

/*definicion de macros*/

  
#define U2X0_8              1U<<U2X0
#define U2X0_16             0


/*variables globales*/
uint8_t data;
char txt[100];
/*************************PROTOTIPO DE FUNCIONES*******************************/


/**
 * @brief CONFIGURA EL USART
 * @param BAUD: velocidad en baudios deseado
 * @return 
 */
void USART_Init(uint32_t BAUD,uint8_t _U2X0);

/**
 * @brief ENVIA UN CARACTER
 * @ch : caracter a enviar
 * @return 
 */
void USART_Transmit(uint8_t ch);
/**
 * @brief Retorna el valor leido del usart
 * @return 
 */
uint8_t USART_Receive(void);

/**
 * @brief EVIA UNA CADENA DE BYTES
 * @param *str: puntero a la cadena a enviar
 * @return 
 */
void USART_PutStr(char *str);
/**
 * funcion principal
 * @return 
 */
int main(void) {
    /*HDW Init*/
    DDRB |= 1U<<DDB5;
    USART_Init(9600,U2X0_16);
    di();
    /*configuracion de interrupciones*/
    //UCSR0B |= 1U<<RXCIE0;
    ei();
    
    sprintf(txt,"configuracion exitosa\r\n");
    USART_PutStr(txt);
    while(1){
        data = USART_Receive();
        if(data == '1')
            PORTB |= 1U<<5;
        else if(data == '2' )
            PORTB = 0;
        
    }
}

/**************************DEFINICION DE FUNCIONES*****************************/
/**
 * @brief CONFIGURA EL USART
 * @param BAUD: velocidad en baudios deseado
 * @return 
 */
void USART_Init(uint32_t BAUD,uint8_t _U2X0){
    uint16_t ubrr = 0;
    if(_U2X0 == U2X0_8){
        UCSR0A |= _U2X0;
        ubrr = F_CPU /(8 * BAUD) - 1;
    }else if(_U2X0 == U2X0_16){
        UCSR0A |= _U2X0;
        ubrr = F_CPU / (16 * BAUD) - 1;
    }
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);
    /*Enable Tx y Rx*/
    UCSR0B |= 1U<<RXEN0 | 1U<<TXEN0;
    /*8-bit format*/
    UCSR0B &=~ (1U<<UCSZ02);
    UCSR0C |= 1U<<UCSZ01 | 1U<<UCSZ00;
    /*ASINCRONOS MODE*/
   
    return;
}

/**
 * @brief ENVIA UN CARACTER
 * @ch : caracter a enviar
 * @return 
 */
void USART_Transmit(uint8_t ch){
    /*wait*/
    while(!(UCSR0A & 1U<<UDRE0));
    /*send data*/
    UDR0 = ch;
    return;
}

ISR(USART_RX_vect)
{
    PORTB ^= 1<<5;
	data = UDR0;
}
/**
 * @brief Retorna el valor leido del usart
 * @return 
 */
uint8_t USART_Receive(void){
    /*wait*/
    while(!(UCSR0A & (1<<RXC0)));

    return UDR0;
}

/**
 * @brief EVIA UNA CADENA DE BYTES
 * @param *str: puntero a la cadena a enviar
 * @return 
 */
void USART_PutStr(char *str){
    
    while(*str){
        USART_Transmit(*str++);
    }
    return;
}

void putch(char data){
    
    USART_Transmit(data);
}
