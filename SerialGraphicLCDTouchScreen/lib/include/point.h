/*
 * point.h
 *
 * Created: 6/14/2016 5:32:18 PM
 *  Author: Monkey
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