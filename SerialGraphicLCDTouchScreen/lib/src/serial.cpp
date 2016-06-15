/*
The MIT License (MIT)
Copyright (c) 2016 Scott McCain

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************************************************
* Filename:	serial.cpp
* Created:	6/14/2016 5:23:41 PM
* Author:	Scott McCain
*******************************************************************************************************************
*
*/

#include "serial.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define RX_BUFFER_SIZE 256	//max 256 bytes due to using char's as index, note this doesn't nee to be set very high if the host interface supports XON/XOFF

//usart variables
char baud_rate = 6;//115200 by default

uint8_t RX_array[RX_BUFFER_SIZE];
volatile uint8_t RX_in = 0;
uint16_t RX_read = 0;
volatile uint8_t RX_bytes=0;	//keeps track of the # of bytes in the RX buffer
volatile uint8_t RX_pause=0;	//flags if RX has been suspended

ISR (USART_RX_vect)//USART Receive Interrupt
{
	cli();//Disable Interrupts

	RX_array[RX_in++] = UDR0;	//get recieved byte
	if (RX_in >= RX_BUFFER_SIZE) RX_in = 0;	//wrap RX_in
	RX_bytes++;			//keep count of bytes recieved
	
	//test for buffer close to full
	if (RX_bytes > (unsigned char)(RX_BUFFER_SIZE *.75))
	{
		USART_send(XOFF);		//send XOFF
		RX_pause=1;		//flag reception suspended
	}
	
	sei();//Enable Interrupts
}


uint8_t RX_available() {
	return RX_bytes;
}

void USART_flush() {
	cli();
	RX_read=0;
	RX_in=0;
	RX_bytes=0;
	sei();

}
 void USART_set_baud(char b)
 {
	 if (b == 1) USART_Init( 1000000/2400-1);//4800
	 else if (b == 2) USART_Init( 1000000/4800-1);//9600
	 else if (b == 3) USART_Init( 1000000/9600-1);//19200
	 else if (b == 4) USART_Init( 1000000/19200-1);//38400
	 else if (b == 5) USART_Init( 1000000/28800-1);//57600
	 else if (b == 6) USART_Init( 1000000/57600-1);//115200
	 
 }

 #define MAX_STR_LEN 128

 char buf[MAX_STR_LEN + 1];

 void USART_Init( unsigned int ubrr)
 {
	 // Set baud rate
	 UBRR0H = (unsigned char)(ubrr>>8);
	 UBRR0L = (unsigned char)ubrr;
	 
	 // Enable receiver and transmitter
	 UCSR0A = (1<<U2X0);
	 UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);	//Enable Interrupts on receive character
	 
	 UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	 sei();
 }

 void USART_send(uint8_t byte)
 {
	 /* Wait for empty transmit buffer */
	 while ( !( UCSR0A & (1<<UDRE0)) );
	 /* Put data into buffer, sends the data */
	 UDR0 = byte;
 }

 uint8_t USART_recv(void)	//retrieves a character from the RX_buffer and does book-keeping
 {
	 uint8_t x;

	 while(RX_bytes==0);				//wait for data to be available
	 x = RX_array[RX_read++];			//get char from buffer and increment read pointer
	 if(RX_read >= RX_BUFFER_SIZE) RX_read = 0;	//if the read pointer reaches the end of the buffer, wrap back to the beginning
	 cli();						//interrupts must be disabled when changing RX_bytes, since it can be chabged here and in the ISR
	 RX_bytes--;
	 sei();
	 
	 //check to see if we need to re-enable reception
	 if (RX_pause==1)		//if USART reception is suspended
	 if (RX_bytes < (unsigned char)(RX_BUFFER_SIZE *.25) )		//if RX_buffer is suitably empty
	 {
		 USART_send(XON);	//resume reception
		 RX_pause=0;	//flag it
	 }

	 return x;
 }

 void USART_printf(const char *fmt, ...) {
	 va_list args;
	 va_start(args, fmt);
	 vsnprintf(buf, sizeof(buf), fmt, args);
	 va_end(args);

	 USART_puts(buf);
 }

 void USART_puts(const char *str) {
	 while(*str) {
		 USART_send(*str++);
	 }
 }
