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
* Filename:	touchscreen.cpp
* Created:	6/14/2016 5:23:41 PM
* Author:	Scott McCain
*******************************************************************************************************************
*
*/ 

#include <avr/io.h>

#include "touchscreen.h"

//-----------------------------------------

#define DDR_XP      DDRC
#define PORT_XP     PORTC
#define XP_BIT      0x08
#define XP_OUTPUT   {DDR_XP|=XP_BIT;}
#define XP_INPUT    {DDR_XP&=~XP_BIT;}
#define XP_HIZ		{DDR_XP&=~XP_BIT;PORT_XP&=~XP_BIT;}
#define XP_HIGH     {PORT_XP|=XP_BIT;}
#define XP_LOW      {PORT_XP&=~XP_BIT;}
#define XP_RISING   {PORT_XP|=XP_BIT;PORT_XP&=~XP_BIT;}

//-----------------------------------------

#define DDR_XM      DDRC
#define PORT_XM     PORTC
#define XM_BIT      0x02
#define XM_OUTPUT   {DDR_XM|=XM_BIT;}
#define XM_INPUT	{DDR_XM&=~XM_BIT;}
#define XM_HIZ		{DDR_XM&=~XM_BIT;PORT_XM&=~XM_BIT;}
#define XM_HIGH     {PORT_XM|=XM_BIT;}
#define XM_LOW      {PORT_XM&=~XM_BIT;}
#define XM_RISING   {PORT_XM|=XM_BIT;PORT_XM&=~XM_BIT;}

//-----------------------------------------

#define DDR_YP      DDRC
#define PORT_YP     PORTC
#define YP_BIT      0x04
#define YP_OUTPUT   {DDR_YP|=YP_BIT;}
#define YP_INPUT	{DDR_YP&=~YP_BIT;}
#define YP_HIZ		{DDR_YP&=~YP_BIT;PORT_YP&=~YP_BIT;}
#define YP_HIGH     {PORT_YP|=YP_BIT;}
#define YP_LOW      {PORT_YP&=~YP_BIT;}
#define YP_RISING   {PORT_YP|=YP_BIT;PORT_YP&=~YP_BIT;}

//-----------------------------------------

#define DDR_YM      DDRC
#define PORT_YM     PORTC
#define YM_BIT      0x01
#define YM_OUTPUT   {DDR_YM|=YM_BIT;}
#define YM_INPUT    {DDR_YM&=~YM_BIT;}
#define YM_HIZ		{DDR_YM&=~YM_BIT;PORT_YM&=~YM_BIT;}
#define YM_HIGH     {PORT_YM|=YM_BIT;}
#define YM_LOW      {PORT_YM&=~YM_BIT;}
#define YM_RISING   {PORT_YM|=YM_BIT;PORT_YM&=~YM_BIT;}

#define TS_MINX 140
#define TS_MAXX 900
#define TS_MINY 120
#define TS_MAXY 940

inline uint16_t read_adc(uint8_t channel);

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void TouchScreen_init() {
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
	ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)
	ADCSRA |= (1<<ADEN);                //Turn on ADC
	ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}


#define NUMSAMPLES 2

TSPoint TouchScreen_getPoint(void) {

	int x, y, z;
	int samples[NUMSAMPLES];
	uint8_t i, valid;
	uint16_t _rxplate = 300;

	valid = 1;

	YP_HIZ
	YM_HIZ

	XP_OUTPUT
	XP_HIGH

	XM_OUTPUT
	XM_LOW

	for (i=0; i<NUMSAMPLES; i++) {
		samples[i] = read_adc(0x02);
	}

	#if NUMSAMPLES == 2
	if (samples[0] != samples[1]) { valid = 0; }
	#endif

	x = (1023-samples[NUMSAMPLES/2]);

	XP_HIZ
	XM_HIZ
	
	YP_OUTPUT
	YP_HIGH

	YM_OUTPUT
	YM_LOW
	
	for (i=0; i<NUMSAMPLES; i++) {
		samples[i] = read_adc(0x01);
	}

	#if NUMSAMPLES == 2
	if (samples[0] != samples[1]) { valid = 0; }
	#endif

	y = (1023-samples[NUMSAMPLES/2]);

	// Set X+ to ground
	XP_OUTPUT
	XP_LOW

	// Set Y- to VCC
	YM_HIGH
	
	// Hi-Z X- and Y+
	YP_HIZ

	int z1 = read_adc(0x01);
	int z2 = read_adc(0x02);

	if (_rxplate != 0) {
		// now read the x
		float rtouch;
		rtouch = z2;
		rtouch /= z1;
		rtouch -= 1;
		rtouch *= x;
		rtouch *= _rxplate;
		rtouch /= 1024;
		
		z = rtouch;
		} else {
		z = (1023-(z2-z1));
	}

	if (! valid) {
		z = 0;
	}

	return TSPoint(map(x, TS_MINX, TS_MAXX, 240, 0), map(y, TS_MINY, TS_MAXY, 320, 0), z);
}



inline uint16_t read_adc(uint8_t channel) {
	ADMUX &= 0xF0;                    //Clear the older channel that was read
	ADMUX |= channel;                //Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);                //Starts a new conversion
	while(ADCSRA & (1<<ADSC));            //Wait until the conversion is done
	return ADCW;                    //Returns the ADC value of the chosen channel
}

