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
* Filename:	lcd.h
* Created:	6/14/2016 5:23:41 PM
* Author:	Scott McCain
*******************************************************************************************************************
*
*/


#ifndef LCD_H_
#define LCD_H_

//Basic Colors
#define RED		0xf800
#define GREEN		0x07e0
#define BLUE		0x001f
#define BLACK		0x0000
#define YELLOW		0xffe0
#define WHITE		0xffff

//Other Colors
#define CYAN		0x07ff
#define BRIGHT_RED	0xf810
#define GRAY1		0x8410
#define GRAY2		0x4208

/**Macro definitions for char display direction**/
#define LCD_DISPLAY_LEFT2RIGHT 0
#define LCD_DISPLAY_DOWN2UP    1
#define LCD_DISPLAY_RIGHT2LEFT 2
#define LCD_DISPLAY_UP2DOWN    3

//TFT resolution 240*320
#define LCD_MIN_X	0
#define LCD_MIN_Y	0
#define LCD_MAX_X	240
#define LCD_MAX_Y	320

void LCD_set_orientation(uint16_t HV);
void LCD_set_display_direct(uint8_t direction);

void LCD_init(void);
void LCD_paint_screen_black(void);

void LCD_setXY(uint16_t poX, uint16_t poY);
void LCD_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void LCD_fillCircle(int16_t poX, int16_t poY, int16_t r, uint16_t color);
void LCD_drawCircle(int16_t poX, int16_t poY, int16_t r, uint16_t color);
void LCD_setPixel(uint16_t poX, uint16_t poY, uint16_t color);
void LCD_drawHorizontalLine(uint16_t poX, uint16_t poY, uint16_t length, uint16_t color);
void LCD_drawVerticalLine(uint16_t poX, uint16_t poY, uint16_t length, uint16_t color);
void LCD_drawRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color);
void LCD_fillRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color);
void LCD_drawChar(char ascii, uint16_t poX, uint16_t poY, uint16_t size, uint16_t fgcolor);

typedef uint8_t (*inputfunction)(void);
void LCD_drawString(const char *string, uint16_t poX, uint16_t poY, uint16_t size, uint16_t fgcolor, inputfunction RX_get_byte);



#endif /* LCD_H_ */