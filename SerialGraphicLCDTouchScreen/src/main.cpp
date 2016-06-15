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
* Filename:	main.cpp
* Created:	6/14/2016 5:23:41 PM
* Author:	Scott McCain
*******************************************************************************************************************
*
*/

#define F_CPU 16000000UL

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "lcd.h"
#include "serial.h"
#include "touchscreen.h"
#include "eeprom.h"

int main(void)
{
	char cmd, subcmd, x, y, v, w, r;	//variables used for loops, buffering of command bytes, counters etc.
	uint16_t color;		//counters for long stuff that may go over 256
	uint8_t pressureThreshhold = 10;

	TouchScreen_init();

	USART_set_baud(6);
	LCD_init();

	//flush any received chars
	USART_flush();

	LCD_drawString("Ready For Command", 0, 10, 1, WHITE, USART_recv);

    /* Replace with your application code */
    while (1)  {
		if(!RX_available()) {

			TSPoint p = TouchScreen_getPoint();

			if(p.z > pressureThreshhold) {
				USART_printf("X = %d\r\n", p.x);
				USART_printf("Y = %d\r\n", p.y);
				USART_printf("Pressure = %d\r\n", p.z);
			}

		} else {
			cmd = USART_recv();
			switch(cmd) {

				case 124:
					subcmd = USART_recv();

					switch(subcmd) {

						case 1:	//clear screen
							LCD_paint_screen_black();
						break;
						//************************************************************************************************************
						case 3:	//draw circle
							x = USART_recv();
							y = USART_recv();
							r = USART_recv();

								// get integer color
							color = USART_recv();
							color = color << 8;
							color |= USART_recv();  

							LCD_drawCircle(x, y, r, color);
						break;
						case 4:
							x = USART_recv();
							y = USART_recv();
							r = USART_recv();

							// get integer color
							color = USART_recv();	//store it and increment RX_read
							color = color << 8;
							color |= USART_recv();

							LCD_fillCircle(x, y, r, color);
						break;
						case 12:	//line
							x = USART_recv();
							y = USART_recv();
							v = USART_recv();
							w = USART_recv();

							// get integer color
							color = USART_recv();
							color = color << 8;
							color |= USART_recv();

							LCD_drawLine(x, y, v, w, color);
						break;
						//************************************************************************************************************
 						case 15:
							x = USART_recv();
							y = USART_recv();
							v = USART_recv();
							w = USART_recv();

							// get integer color
							color = USART_recv();
							color = color << 8;
							color |= USART_recv();

							LCD_drawRectangle(x, y, v, w, color);
						break;
						//************************************************************************************************************
						case 16:		//set pixel
							x = USART_recv();
							y = USART_recv();

							color = USART_recv();
							color = color << 8;
							color |= USART_recv();
 					
					
							LCD_setPixel(x, y, color);
						break;
						case 18:
							x = USART_recv();
							y = USART_recv();
							v = USART_recv();
							w = USART_recv();

							// get integer color
							color = USART_recv();
							color = color << 8;
							color |= USART_recv();

							LCD_fillRectangle(x, y, v, w, color);
						break;
						case 20: // draw text
							x = USART_recv();

							y = USART_recv();

							// size
							r = USART_recv();

							// get integer color
							color = USART_recv();	//store it and increment RX_read
							color = color << 8;
							color |= USART_recv();

							// send string end with 0
							LCD_drawString(NULL, x, y, r, color, USART_recv);
						break;
					}
				break;
			}
		}
    }
}


