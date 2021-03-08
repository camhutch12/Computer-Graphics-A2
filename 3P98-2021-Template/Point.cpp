#include "Point.h"
#include <cmath>
/*
Written By:
Cameron Hutchings (6427892)
Daniel Gannage (6368898)
*/

/*
Constructor initializes the variables
*/
Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

/*
Empty constructor
*/
Point::Point() {
	this->x = 0;
	this->y = 0;
}


/*
This method calculates the euclidean distance beween two points
*/
double Point::euclidenaDistance(int x, int y)
{
	int dx = x - this->x;
	int dy = y - this->y;
	return (double)(pow(dx + dy, 2) * 0.5);
}
