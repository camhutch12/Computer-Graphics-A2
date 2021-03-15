#pragma once
#include "Point.h"

/*
Written By:
Cameron Hutchings (6427892)
Daniel Gannage (6368898)
*/

/*
Header file for Line.cpp
*/
class Line
{
public:
	double distance{};
	Point pt1;
	Point pt2;
	Line();
	Line(Point p1, Point p2);
	void getDistance(void);
};

