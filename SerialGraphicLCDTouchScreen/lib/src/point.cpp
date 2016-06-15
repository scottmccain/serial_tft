/*
 * point.cpp
 *
 * Created: 6/14/2016 5:32:00 PM
 *  Author: Monkey
 */ 

 #include "point.h"

 TSPoint::TSPoint(void) {
	 x = y = 0;
 }

 TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
	 x = x0;
	 y = y0;
	 z = z0;
 }

 inline bool TSPoint::operator==(TSPoint p1) {
	 return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
 }

 inline bool TSPoint::operator!=(TSPoint p1) {
	 return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
 }
