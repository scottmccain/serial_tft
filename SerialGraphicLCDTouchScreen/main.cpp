/*
 * TouchScreenTest.cpp
 *
 * Created: 6/9/2016 11:42:21 PM
 * Author : Monkey
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
	char x, y, z, temp, q, r;	//variables used for loops, buffering of command bytes, counters etc.
	uint16_t count, count2, color;		//counters for long stuff that may go over 256
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
			x = USART_recv();
			switch(x) {		//switch based on received character

				//--------------------------------------------------------------------------------------------------------------------
				case 124:					//received character indicated a coming command
				y = USART_recv();

				switch(y) {				//switch to which command was sent
					case 1:	//clear screen
						LCD_paint_screen_black();
					break;
					//************************************************************************************************************
					case 3:	//draw circle
						//need 4 bytes, RX_read already pointing at the first
							//wait for byte (x coord of circle)
						y = USART_recv();	//get char from buffer
							//wait for byte (y coord of circle)
						z = USART_recv();	//get char from buffer
							//wait for byte (r of circle)
						r = USART_recv();	//store it and increment RX_read
							//wait for byte (S_R of circle)

							// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();  

						LCD_drawCircle(y, z, r, color);
					break;
					case 4:
						y = USART_recv();	//get char from buffer
						//wait for byte (y coord of circle)
						z = USART_recv();	//get char from buffer
						//wait for byte (r of circle)
						r = USART_recv();	//store it and increment RX_read
						//wait for byte (S_R of circle)

						// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();

						LCD_fillCircle(y, z, r, color);
					break;
					//************************************************************************************************************
					case 12:	//line
						//need 5 bytes, RX_read already pointing at the first
					
							//wait for byte (x coord of start)
						y = USART_recv();	//store it and increment RX_read
 							//wrap if necessary
						
							//wait for byte (y coord of start)
						z = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (x coord of end)
						temp = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (y coord of end)
						q = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

						// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();

						LCD_drawLine(y ,z , temp, q, color);
					break;
					//************************************************************************************************************
 					case 15:	//draw box
						//need 5 bytes, RX_read already pointing at the first
					
							//wait for byte (x coord of corner1)
						y = USART_recv();	//store it and increment RX_read
 							//wrap if necessary
						
							//wait for byte (y coord of corner1)
						z = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (x coord of corner2)
						temp = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (y coord of corner2)
						q = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

						// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();

						LCD_drawRectangle(y ,z , temp, q, color);
					break;
					//************************************************************************************************************
					case 16:		//set pixel
						//need 3 bytes, RX_read already pointing at the first
					
							//wait for byte (x coord of pixel)
						z = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (y coord of pixel)d
						temp = USART_recv();
 					

							//wait for byte (S_R of pixel)
						// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();
 					
					
						LCD_setPixel(z, temp, color);		//draw the pixel
					break;
					//************************************************************************************************************
					// ASCII 17 is XON so we can't use it for a command
					case 18:	//draw filled box
						//need 5 bytes, RX_read already pointing at the first
					
							//wait for byte (x coord of corner1)
						y = USART_recv();	//store it and increment RX_read
 							//wrap if necessary
						
							//wait for byte (y coord of corner1)
						z = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (x coord of corner2)
						temp = USART_recv();	//store it and increment RX_read
 							//wrap if necessary

							//wait for byte (y coord of corner2)
						q = USART_recv();	//store it and increment RX_read
 							//wrap if necessary
					
							//wait for byte (fill)
						// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();
 							//wrap if necessary


						LCD_fillRectangle(y ,z , temp, q, color);
					break;
					case 20: // draw text
						y = USART_recv();

						z = USART_recv();

						// size
						temp = USART_recv();

						// get integer color
						color = USART_recv();	//store it and increment RX_read
						color = color << 8;
						color |= USART_recv();

						// send string end with 0
						LCD_drawString(NULL, y, z, temp, color, USART_recv);
					break;
				}
				break;
			}
		}
    }
}


