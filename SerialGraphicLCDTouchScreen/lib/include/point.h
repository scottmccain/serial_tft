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


#ifndef POINT_H_
#define POINT_H_

#include <inttypes.h>

struct TSPoint {
	public:
	TSPoint(void);
	TSPoint(int16_t x, int16_t y, int16_t z);
	
	inline bool operator==(TSPoint);
	inline bool operator!=(TSPoint);

	int16_t x, y, z;
};



#endif /* POINT_H_ */