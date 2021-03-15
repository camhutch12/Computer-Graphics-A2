#include "Line.h"
#include "Point.h"

/*
Written By:
Cameron Hutchings (6427892)
Daniel Gannage (6368898)
*/

/*
Constructor initializes the variables
*/
Line::Line(Point p1, Point p2) {

	this->pt1 = p1;
	this->pt2 = p2;
	this->getDistance();
};

void Line::getDistance(void) {
	this->distance = this->pt1.euclidenaDistance(this->pt2.x, this->pt2.y);
}

/*
Empty constructor
*/
Line::Line() {};


