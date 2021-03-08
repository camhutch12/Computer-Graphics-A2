#pragma once 

/*
Written By:
Cameron Hutchings (6427892)
Daniel Gannage (6368898)
*/

/*
Header file for Point.cpp
*/
class Point {

public:
	int x{};
	int y{};

	Point(int x, int y);

	Point();

	double euclidenaDistance(int x, int y);

};