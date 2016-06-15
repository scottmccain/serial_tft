/*
 * LCD.c
 *
 * Created: 6/14/2016 3:52:07 PM
 *  Author: Monkey
 */ 

 #include <stdlib.h>
 #include <stdint.h>
 #include <avr/io.h>
 #include <util/delay.h>

 #include <avr/pgmspace.h>

 #include "lcd.h"

 volatile uint8_t display_direct;

 //#define SPRITE_SIZE 34		//needs to be at least 22 to hold sparkfun logo
 //#define NUM_SPRITES 8		//make sure that the memory allocated to sprites (SPRITE_SIZE*NUM_SPRITES) doesn't over fill the device (it only has 1K for all variables)
//
 ////logo modified to use with bitblt, and placed in first sprite
 //uint16_t sprite[SPRITE_SIZE*NUM_SPRITES]={
	 //0x0A,0x10,0x8000,0xc000,0x4000,0x0c00,0x3e00,0xfe00,0xf200,	//array to hold sprites, each is SPRITE_BYTES long, each sprite has [width],[height],[bunch O' bytes for bitblk]
	 //0xe000,0xf000,0xe000,0xffff,0x7fff,0x3fff,0x1fff,		//user must make sure data is ok, code does not check
 //0x1fff,0x1fff,0x1fff,0x0fff,0x0700,0x0300};


 inline void exit_standby(void);

 inline void send_command(uint16_t index);
 inline void send_data(uint16_t data);
 inline void push_data(uint8_t data);
 inline uint8_t get_data(void);
 inline uint16_t read_register(uint8_t index);

 inline void all_pin_input(void);
 inline void all_pin_output(void);
 inline void all_pin_low(void);

 //TFT resolution 240*320
 #define MIN_X	0
 #define MIN_Y	0
 #define MAX_X	240
 #define MAX_Y	320

 //========================================
 #define DDR_CS      DDRB
 #define PORT_CS     PORTB
 #define CS_BIT      0x04
 #define CS_OUTPUT   {DDR_CS|=CS_BIT;}
 #define CS_HIGH     {PORT_CS|=CS_BIT;}
 #define CS_LOW      {PORT_CS&=~CS_BIT;}

 //-----------------------------------------

 #define DDR_RS      DDRB
 #define PORT_RS     PORTB
 #define RS_BIT      0x08
 #define RS_OUTPUT   {DDR_RS|=RS_BIT;}
 #define RS_HIGH     {PORT_RS|=RS_BIT;}
 #define RS_LOW      {PORT_RS&=~RS_BIT;}

 //-----------------------------------------

 #define DDR_WR      DDRB
 #define PORT_WR     PORTB
 #define WR_BIT      0x10
 #define WR_OUTPUT   {DDR_WR|=WR_BIT;}
 #define WR_HIGH     {PORT_WR|=WR_BIT;}
 #define WR_LOW      {PORT_WR&=~WR_BIT;}
 #define WR_RISING   {PORT_WR|=WR_BIT;PORT_WR&=~WR_BIT;}

 //-----------------------------------------

 #define DDR_RD      DDRB
 #define PORT_RD     PORTB
 #define RD_BIT      0x20
 #define RD_OUTPUT   {DDR_RD|=RD_BIT;}
 #define RD_HIGH     {PORT_RD|=RD_BIT;}
 #define RD_LOW      {PORT_RD&=~RD_BIT;}
 #define RD_RISING   {PORT_RD|=RD_BIT;PORT_RD&=~RD_BIT;}


 extern unsigned char simpleFont[][8];


 inline void push_data(uint8_t data)
 {
	 all_pin_low();
	 PORTD |= (data<<2);
	 PORTB |= (data>>6);
 }

 inline uint8_t get_data(void)
 {
	 uint8_t data=0;
	 _delay_ms(1);
	 data |= ((PIND&0xfc)>>2);
	 data |= ((PINB&0x03)<<6);
	 return data;
 }

 inline void send_command(uint16_t index)
 {
	 CS_LOW;
	 RS_LOW;
	 RD_HIGH;
	 WR_HIGH;

	 WR_LOW;
	 push_data(0);
	 WR_HIGH;
	 WR_LOW;
	 push_data(index&0xff);
	 WR_HIGH;

	 CS_HIGH;
 }

 inline void send_data(uint16_t data)
 {
	 CS_LOW;
	 RS_HIGH;
	 RD_HIGH;

	 WR_LOW;
	 push_data((data&0xff00)>>8);
	 WR_HIGH;

	 WR_LOW;
	 push_data(data&0xff);
	 WR_HIGH;

	 CS_HIGH;
 }

 inline uint16_t read_register(uint8_t index)
 {
	 uint16_t data=0;

	 CS_LOW;
	 RS_LOW;
	 RD_HIGH;

	 all_pin_output();

	 WR_LOW;
	 push_data(0);
	 WR_HIGH;

	 WR_LOW;
	 push_data(index);
	 WR_HIGH;

	 all_pin_input();
	 all_pin_low();
	 RS_HIGH;

	 RD_LOW;
	 RD_HIGH;
	 data |= get_data()<<8;

	 RD_LOW;
	 RD_HIGH;
	 data |= get_data();

	 CS_HIGH;
	 all_pin_output();
	 return data;
 }

 inline void all_pin_low(void)
 {
	 PORTD &=~ 0xfc;
	 PORTB &=~ 0x03;
 }

 inline void all_pin_input(void)
 {
	 DDRD &=~ 0xfc;
	 DDRB &=~ 0x03;
 }

 inline void all_pin_output(void)
 {
	 DDRD |= 0xfc;
	 DDRB |= 0x03;
 }

 void LCD_init (void)
 {
	 CS_OUTPUT;
	 RD_OUTPUT;
	 WR_OUTPUT;
	 RS_OUTPUT;

	 all_pin_output();
	 all_pin_low();

	 _delay_ms(100);


	 send_command(0x0001);
	 send_data(0x0100);
	 send_command(0x0002);
	 send_data(0x0700);
	 send_command(0x0003);
	 send_data(0x1030);
	 send_command(0x0004);
	 send_data(0x0000);
	 send_command(0x0008);
	 send_data(0x0302);
	 send_command(0x000A);
	 send_data(0x0000);
	 send_command(0x000C);
	 send_data(0x0000);
	 send_command(0x000D);
	 send_data(0x0000);
	 send_command(0x000F);
	 send_data(0x0000);

	 _delay_ms(100);

	 send_command(0x0030);
	 send_data(0x0000);
	 send_command(0x0031);
	 send_data(0x0405);
	 send_command(0x0032);
	 send_data(0x0203);
	 send_command(0x0035);
	 send_data(0x0004);
	 send_command(0x0036);
	 send_data(0x0B07);
	 send_command(0x0037);
	 send_data(0x0000);
	 send_command(0x0038);
	 send_data(0x0405);
	 send_command(0x0039);
	 send_data(0x0203);
	 send_command(0x003c);
	 send_data(0x0004);
	 send_command(0x003d);
	 send_data(0x0B07);
	 send_command(0x0020);
	 send_data(0x0000);
	 send_command(0x0021);
	 send_data(0x0000);
	 send_command(0x0050);
	 send_data(0x0000);
	 send_command(0x0051);
	 send_data(0x00ef);
	 send_command(0x0052);
	 send_data(0x0000);
	 send_command(0x0053);
	 send_data(0x013f);

	 _delay_ms(100);

	 send_command(0x0060);
	 send_data(0xa700);
	 send_command(0x0061);
	 send_data(0x0001);
	 send_command(0x0090);
	 send_data(0x003A);
	 send_command(0x0095);
	 send_data(0x021E);
	 send_command(0x0080);
	 send_data(0x0000);
	 send_command(0x0081);
	 send_data(0x0000);
	 send_command(0x0082);
	 send_data(0x0000);
	 send_command(0x0083);
	 send_data(0x0000);
	 send_command(0x0084);
	 send_data(0x0000);
	 send_command(0x0085);
	 send_data(0x0000);
	 send_command(0x00FF);
	 send_data(0x0001);
	 send_command(0x00B0);
	 send_data(0x140D);
	 send_command(0x00FF);
	 send_data(0x0000);
	 _delay_ms(100);
	 send_command(0x0007);
	 send_data(0x0133);
	 _delay_ms(50);
	 exit_standby();
	 send_command(0x0022);

	 //paint screen black
	 LCD_paint_screen_black();
 }

 void LCD_paint_screen_black(void)
 {
	 for(unsigned char i=0;i<2;i++)
	 {
		 for(unsigned int f=0;f<38400;f++)
		 {
			 send_data(BLACK);
		 }
	 }
 }

 void exit_standby(void)
 {
	 send_command(0x0010);
	 send_data(0x14E0);
	 _delay_ms(100);
	 send_command(0x0007);
	 send_data(0x0133);
 }

 void LCD_set_orientation(uint16_t HV)//horizontal or vertical
 {
	 send_command(0x03);
	 if(HV==1)//vertical
	 {
		 send_data(0x5038);
	 }
	 else//horizontal
	 {
		 send_data(0x5030);
	 }
	 send_command(0x0022); //Start to write to display RAM
 }

 void LCD_set_display_direct(uint8_t direction)
 {
	 display_direct = direction;
 }

 void LCD_setXY(uint16_t poX, uint16_t poY)
 {
	 send_command(0x0020);//X
	 send_data(poX);
	 send_command(0x0021);//Y
	 send_data(poY);
	 send_command(0x0022);//Start to write to display RAM
 }

 void LCD_setPixel(uint16_t poX, uint16_t poY, uint16_t color)
 {
	 LCD_setXY(poX,poY);
	 send_data(color);
 }

 void LCD_drawCircle(int16_t poX, int16_t poY, int16_t r, uint16_t color)
 {
	 int16_t x = -r, y = 0, err = 2-2*r, e2;
	 do {
		 LCD_setPixel(poX-x, poY+y,color);
		 LCD_setPixel(poX+x, poY+y,color);
		 LCD_setPixel(poX+x, poY-y,color);
		 LCD_setPixel(poX-x, poY-y,color);
		 e2 = err;
		 if (e2 <= y) {
			 err += ++y*2+1;
			 if (-x == y && e2 <= x) e2 = 0;
		 }
		 if (e2 > x) err += ++x*2+1;
	 } while (x <= 0);
 }

 void LCD_fillCircle(int16_t poX, int16_t poY, int16_t r, uint16_t color)
 {
	 int x = -r, y = 0, err = 2-2*r, e2;
	 do {

		 LCD_drawVerticalLine(poX-x,poY-y,2*y,color);
		 LCD_drawVerticalLine(poX+x,poY-y,2*y,color);

		 e2 = err;
		 if (e2 <= y) {
			 err += ++y*2+1;
			 if (-x == y && e2 <= x) e2 = 0;
		 }
		 if (e2 > x) err += ++x*2+1;
	 } while (x <= 0);

 }


 void LCD_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
 {
	 int x = x1-x0;
	 int y = y1-y0;
	 int dx = abs(x), sx = x0<x1 ? 1 : -1;
	 int dy = -abs(y), sy = y0<y1 ? 1 : -1;
	 int err = dx+dy, e2; /* error value e_xy */
	 for (;;){ /* loop */
		 LCD_setPixel(x0,y0,color);
		 e2 = 2*err;
		 if (e2 >= dy) { /* e_xy+e_x > 0 */
			 if (x0 == x1) break;
			 err += dy; x0 += sx;
		 }
		 if (e2 <= dx) { /* e_xy+e_y < 0 */
			 if (y0 == y1) break;
			 err += dx; y0 += sy;
		 }
	 }
 }


 void LCD_drawHorizontalLine(uint16_t poX, uint16_t poY, uint16_t length, uint16_t color)
 {
	 LCD_setXY(poX,poY);
	 LCD_set_orientation(0);
	 if(length+poX>MAX_X)
	 {
		 length=MAX_X-poX;
	 }
	 for(unsigned int i=0;i<length;i++)
	 {
		 send_data(color);
	 }
 }


 void LCD_drawVerticalLine(uint16_t poX, uint16_t poY, uint16_t length, uint16_t color)
 {
	 LCD_setXY(poX,poY);
	 LCD_set_orientation(1);
	 if(length+poY>MAX_Y)
	 {
		 length=MAX_Y-poY;
	 }

	 for(unsigned int i=0;i<length;i++)
	 {
		 send_data(color);
	 }
 }

 void LCD_drawRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color)
 {
	 LCD_drawHorizontalLine(poX, poY, length, color);
	 LCD_drawHorizontalLine(poX, poY+width, length, color);

	 LCD_drawVerticalLine(poX, poY, width,color);
	 LCD_drawVerticalLine(poX + length, poY, width,color);
 }

 void LCD_fillRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color)
 {
	 for(unsigned int i=0;i<width;i++)
	 {
		 if(display_direct == LCD_DISPLAY_LEFT2RIGHT)
		 LCD_drawHorizontalLine(poX, poY+i, length, color);
		 else if (display_direct ==  LCD_DISPLAY_DOWN2UP)
		 LCD_drawHorizontalLine(poX, poY-i, length, color);
		 else if(display_direct == LCD_DISPLAY_RIGHT2LEFT)
		 LCD_drawHorizontalLine(poX, poY-i, length, color);
		 else if(display_direct == LCD_DISPLAY_UP2DOWN)
		 LCD_drawHorizontalLine(poX, poY+i, length, color);
	 }
 }

 void LCD_drawChar(char ascii, uint16_t poX, uint16_t poY, uint16_t size, uint16_t fgcolor)
 {
	 
	 LCD_setXY(poX,poY);
	 
	 if((ascii < 0x20)||(ascii > 0x7e))//Unsupported char.
	 {
		 ascii = '?';
	 }
	 for(unsigned char i=0;i<8;i++)
	 {
		 unsigned char temp = pgm_read_byte(&simpleFont[ascii-0x20][i]);
		 for(unsigned char f=0;f<8;f++)
		 {
			 if((temp>>f)&0x01)
			 {
				 if(display_direct == LCD_DISPLAY_LEFT2RIGHT)
				 LCD_fillRectangle(poX+i*size, poY+f*size, size, size, fgcolor);
				 else if(display_direct == LCD_DISPLAY_DOWN2UP)
				 LCD_fillRectangle(poX+f*size, poY-i*size, size, size, fgcolor);
				 else if(display_direct == LCD_DISPLAY_RIGHT2LEFT)
				 LCD_fillRectangle(poX-i*size, poY-f*size, size, size, fgcolor);
				 else if(display_direct ==LCD_DISPLAY_UP2DOWN)
				 LCD_fillRectangle(poX-f*size, poY+i*size, size, size, fgcolor);
			 }

		 }
	 }
 }

 void LCD_drawString(const char *string, uint16_t poX, uint16_t poY, uint16_t size, uint16_t fgcolor, inputfunction RX_get_byte)
 {
	 char x;

	 if(!string) {
		 while((x = RX_get_byte())) {
			 LCD_drawChar(x, poX, poY, size, fgcolor);

			 if(display_direct == LCD_DISPLAY_LEFT2RIGHT)
			 {
				 if(poX < MAX_X)
				 {
					 poX+=8*size; // Move cursor right
				 }
			 }
			 else if(display_direct == LCD_DISPLAY_DOWN2UP)
			 {
				 if(poY > 0)
				 {
					 poY-=8*size; // Move cursor right
				 }
			 }
			 else if(display_direct == LCD_DISPLAY_RIGHT2LEFT)
			 {
				 if(poX > 0)
				 {
					 poX-=8*size; // Move cursor right
				 }
			 }
			 else if(display_direct == LCD_DISPLAY_UP2DOWN)
			 {
				 if(poY < MAX_Y)
				 {
					 poY+=8*size; // Move cursor right
				 }
			 }
		 }
		 } else {
		 while(*string)
		 {
			 for(unsigned char i=0;i<8;i++)
			 {
				 LCD_drawChar(*string, poX, poY, size, fgcolor);
			 }
			 string++;
			 if(display_direct == LCD_DISPLAY_LEFT2RIGHT)
			 {
				 if(poX < MAX_X)
				 {
					 poX+=8*size; // Move cursor right
				 }
			 }
			 else if(display_direct == LCD_DISPLAY_DOWN2UP)
			 {
				 if(poY > 0)
				 {
					 poY-=8*size; // Move cursor right
				 }
			 }
			 else if(display_direct == LCD_DISPLAY_RIGHT2LEFT)
			 {
				 if(poX > 0)
				 {
					 poX-=8*size; // Move cursor right
				 }
			 }
			 else if(display_direct == LCD_DISPLAY_UP2DOWN)
			 {
				 if(poY < MAX_Y)
				 {
					 poY+=8*size; // Move cursor right
				 }
			 }
		 }
	 }
 }
