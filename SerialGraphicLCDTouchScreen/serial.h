/*
 * serial.h
 *
 * Created: 6/14/2016 5:00:46 PM
 *  Author: Monkey
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