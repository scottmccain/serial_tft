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


#ifndef SERIAL_H_
#define SERIAL_H_

#include <inttypes.h>

#define XOFF 0X13
#define XON 0x11

void USART_Init( unsigned int ubrr);

//serial comms functions
void USART_send(uint8_t b);
void USART_puts(const char *str);
void USART_printf(const char *fmt, ...);
void USART_set_baud(char b);
uint8_t USART_recv(void);  //retrieves a bye from the serial port, blocks till data is available
void USART_flush(void);
uint8_t RX_available();



#endif /* SERIAL_H_ */