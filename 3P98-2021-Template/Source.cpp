

#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <malloc.h>
#include <iostream>
#include <math.h>
#include "Point.h";
#include <freeglut.h>
#include <vector>
#include <stdio.h>
#include "Line.h"
#include "Source.h"
using namespace std;

/*
Written By:
Cameron Hutchings (6427892)
Daniel Gannage (6368898)
*/

/*
The global structure
*/

bool notInStack(Line* line);



typedef struct {

	int w, h;
	vector<Point*> pointArray;
	vector<Line*> lines;

	bool mouseClick;	// registers whether the user is in mouse click mode or random points mode
	int x, y;			// mouse click coordinates
	int pointsCounter;	// registers how many times 100 random points have been selected

} glob;
glob global;

// Enumerate menu items
enum { MENU_FILTER, MENU_SAVE, MENU_TRIANGLE, MENU_QUIT };

/*
Draw the image - it is already in the format openGL requires for glDrawPixels
*/
void display_image(void)
{
	//glDrawPixels(global.w, global.h, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)global.data);
	glFlush();
}

/*
This method checks to see if a given point already exists in the vector
*/
bool doesPointExist(vector<Point*> points, int x2, int y2) {

	// check if point exists
	// iterate through points in vector
	for (auto p = points.begin(); p != points.end(); ++p) {
		// get next point
		Point* pt = *p;
		// get point components
		int x1 = pt->x;
		int y1 = pt->y;

		// check if both points overlap
		if ((x2 > x1 - 10 && x2 < x1 + 10) && (y2 > y1 - 10 && y2 < y1 + 10)) {
			return true;
		}
		// check if both points are the same
		//if (x2 == x1 && y2 == y1) {
		//	return true; // this point already exists!
		//}
	}
	return false; // all good, point does not exist
}

/*
This initializes a
*/
void initPoints(void) {

	srand(time(NULL));
	for (int i = 0; i < global.pointsCounter; i++) {

		// generate random number between 100 and 400
		int min = 10;
		int max = global.h - 50;

		int randX = min + rand() % (max - min + 1);


		int randY = min + rand() % (max - min + 1);


		// check to see if point already exists
		if (doesPointExist(global.pointArray, randX, randY) == false) {
			// add point to vector
			global.pointArray.push_back(new Point(randX, randY));
		}
		else {
			// create new points

			randX = min + rand() % (max - min + 1);
			randY = min + rand() % (max - min + 1);
			// add point to vector
			global.pointArray.push_back(new Point(randX, randY));
			//TODO:: CHECK AGAIN
		}

	}

}

/*
This method checks if the new line is connecting to a duplicate point
*/
bool isInHull(Point* pt1, Point* pt2, vector<Line*> currentLine) {

	// check if line size is 0
	if (currentLine.size() == 0) {
		return true;

	}
	else {
		for (Line* l : currentLine)
		{
			// check if p1 has same x and y coordinate a pt1
			if ((pt1->x == l->pt1.x) && (pt1->y == l->pt1.y))
			{
				return false;
			}
			if ((pt2->x == l->pt2.x) && (pt2->y == l->pt2.y)) {
				return false;
			}
		}
		return true;
	}
}

vector<int> getDirectionBetweenTwoPoints(Point* tail, Point* tip) {
	vector<int> v;
	v.push_back(tail->x - tip->x);
	v.push_back(tail->y - tip->y);
	return v;
}
int getAngleOfVector(vector<int>& v) {
	// TODO: Calculate angle of vector
	if (v[0] == 0)
	{
		return 0;
	}
	double angleRad = atan(v[1] / v[0]);
	double angleDegree = angleRad * (180 / 3.141592);
	//cout << angleDegree << endl;
	return (int)angleDegree;
	// if the angle is greater than 45 degees then return false 

	// otherwise we need to return true

}

/*
This method grabs the lowest point on the y-axis and chooses that as the starting point
*/
int getStartingPoint() {

	int index = -1;
	int minY = 1000;

	// go through all points in vector
	for (size_t i = 0; i < global.pointArray.size(); i++)
	{
		Point* pt = global.pointArray[i];
		int x = pt->x;
		int y = pt->y;

		if (minY > y) {
			minY = y;
			index = i;
		}

	}

	return index;
}

/*
This method creates a new line between the starting
point and the next point in the vector and uses the convex hull algorithm
to check whether the line is valid
*/
bool checkLine(Point* currentPoint, Point* nextPoint) {

	// current line in question
	Line* line = new Line(*currentPoint, *nextPoint);

	// check if line is in queue, if not, do the convex hull line check
	if (notInStack(line)) {

		//get coordinates
		int x1 = currentPoint->x; // current point
		int y1 = currentPoint->y;

		int x2 = nextPoint->x; // next point
		int y2 = nextPoint->y;

		int a = y1 - y2;
		int b = x2 - x1;
		int c = (x1 * y2) - (y1 * x2);

		int pos = 0;
		int neg = 0;
		int onLine = 0;

		// get all points in vector except current/next point involved in the line in question
		for (size_t i = 0; i != global.pointArray.size(); i++) {
			// get next point
			Point* pt = global.pointArray[i];
			// get point components
			int x = pt->x;
			int y = pt->y;

			// check if points being used are duplicates
			if ((x == x1 && y == y1) || (x == x2 && y == y2)) {
				//skip
			}
			else {

				// convex hull: create imaginary line 'd' between two points
				// 0 = ax+by+c (our line where all points compared to are on the line)
				// d = ax+by+c 
				double d = ((a * x) + (b * y) + c) / sqrt((a * a) + (b * b));

				// check if all points are above or below line (check if d is positive or negative)
				// if d=0, then point is on line
				if (d > 0) {
					pos++;
				}
				else if (d < 0) {
					neg++;
				}
				else {
					// d = 0
					onLine++;
				}
			}
		}

		// check if all points are on one side of the line (either pos or neg is 0)
		if (pos == 0 || neg == 0) {
			return true;  // valid line in convex hull
		}
		else {
			return false; // invalid convex hull line
		}

	}

}

/*
This method goes through all the points that are pssed in as a
vector and finds the set of x,y coordinates with the lowest y value. Then
it draws this point to the canvas
*/
void drawInitialPoints(std::vector<Point*>& pointArray, int start)
{

	// iterate through points in vector
	for (size_t i = 0; i != pointArray.size(); i++) {

		// get next point
		Point* pt = pointArray[i];
		// get point components
		int x = pt->x;
		int y = pt->y;

		// draw point
		glPointSize(1);
		glBegin(GL_POINTS);
		if (start == i) {
			glColor3f(0.0, 0.0, 1.0);
		}
		else {
			glColor3f(1.0, 0, 0);
		}
		glVertex2i(x, y);
		glFlush();
		glEnd();
	}
}

/*
This method goes through all the lines that are pssed in as a
stack and then draws those to the canvas in the stack order
*/
void drawLines()
{
	// go through queue and delete points that go to itself (null lines)
	for (int j = 0; j < global.lines.size(); j++) {
		for (int i = 0; i < global.lines.size(); i++) {

			// get line at top of stack
			Line* line = global.lines[i];

			// point 1
			int x1 = line->pt1.x;
			int y1 = line->pt1.y;
			// point 2
			int x2 = line->pt2.x;
			int y2 = line->pt2.y;

			// if line goes to itself
			if ((x1 == x2 && y1 == y2)) {
				// delete line from list
				global.lines.erase(global.lines.begin() + i);	// remove line from active list of lines
			}

		}
	}

	// go through queue
	for (int i = 0; i < global.lines.size(); i++) {

		// get line at top of stack
		Line* line = global.lines[i];

		// point 1
		int x1 = line->pt1.x;
		int y1 = line->pt1.y;
		// point 2
		int x2 = line->pt2.x;
		int y2 = line->pt2.y;

		printf("\n%d : (%d,%d) : (%d,%d)", i, x1, y1, x2, y2);

		// print line
		glPointSize(10);
		glBegin(GL_LINES);
		glColor3f(0, 1.0, 0);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
		glFlush();
		glEnd();
	}

	// go through queue
	for (int i = 0; i < global.lines.size(); i++) {

		// get line at top of stack
		Line* line = global.lines[i];

		// point 1
		int x1 = line->pt1.x;
		int y1 = line->pt1.y;
		// point 2
		int x2 = line->pt2.x;
		int y2 = line->pt2.y;

		// delete point from point vector
		// go through point vector
		for (int j = 0; j < global.pointArray.size(); j++) {
			int x = global.pointArray[j]->x;
			int y = global.pointArray[j]->y;
			// does the point match the line point
			if ((x == x2 && y == y2) || (x == x1 && y == y1)) {
				//printf("\n%d : (%d,%d)", j, global.pointArray[j]->x, global.pointArray[j]->y);
				//erase that point
				global.pointArray.erase(global.pointArray.begin() + j);
			}
		}

		// delete line from list
		global.lines.erase(global.lines.begin() + i);	// remove line from active list of lines
	}
}

bool notInStack(Line* line) {

	// check if stack of current poitns is empty
	if (global.lines.empty()) {
		return true;
	}
	else {
		// go through stack of current points
		for (Line* ele : global.lines) {

			int x1 = ele->pt1.x;
			int y1 = ele->pt1.y;

			int x2 = line->pt1.x;
			int y2 = line->pt1.y;

			int x3 = ele->pt2.x;
			int y3 = ele->pt2.y;

			int x4 = line->pt2.x;
			int y4 = line->pt2.y;

			// dont connect to points in stack
			if ((x1 == x2 && y1 == y2) && (x3 == x4 && y3 == y4)) {
				return false;
			}
		}
		return true;
	}

}

/*
This method checks whether the point from p1 to p2 is the smallest euclidean
distance compared to any other point in relation to p1
*/
bool closestPoint(Point* p1, Point* p2, vector<Point*> checkedPoints) {

	// calculate distance for current point set
	double d = p1->euclidenaDistance(p2->x, p2->y);

	// go through points vector
	for (size_t i = 0; i < global.pointArray.size(); i++) {

		if (checkedPoints.size() != i + 1) {

			// check if this is the smallest distance out of points that have never been checked
			if (checkedPoints[i]->x != global.pointArray[i]->x && checkedPoints[i]->y != global.pointArray[i]->y) {

				// check whether the current point is the smallest euclidean distanc
				double newD = p1->euclidenaDistance(global.pointArray[i]->x, global.pointArray[i]->y);
				// only check distance to points other than p1 (initial point)
				if (global.pointArray[i]->x == p1->x && global.pointArray[i]->y == p1->y)
				{

				}
				else if (newD < d) {
					return false; // not the smallest distance
				}

			}

		}
		else {
			// check whether the current point is the smallest euclidean distanc
			double newD = p1->euclidenaDistance(global.pointArray[i]->x, global.pointArray[i]->y);
			// only check distance to points other than p1 (initial point)
			if (global.pointArray[i]->x == p1->x && global.pointArray[i]->y == p1->y)
			{

			}
			else if (newD < d) {
				return false; // not the smallest distance
			}
		}

	}
	// this point set is the smallest euclidean distance
	return true;
}

/*
This method does the convex hull algorithm by brute force
*/
void convexHull() {

	Point* pt;
	Point* pt2;

	// draw line from previous point to next point in vector
	for (size_t i = 0; i < global.pointArray.size(); i++) {

		// get current point
		pt = global.pointArray[i];

		for (size_t j = 0; j < global.pointArray.size(); j++) {

			// get next point
			pt2 = global.pointArray[j];

			// do the hull line check
			if (checkLine(pt, pt2)) {

				// add line to list of lines
				Line* line = new Line(*pt, *pt2);
				global.lines.push_back(line);

			}
		}
	}
}

/*
 This method draws a polygon using open GL
 */
void draw_polygon(void) {

	convexHull();
	drawLines();
	glutPostRedisplay();
	//printf("size of points array: %d",global.pointArray.size());
}

/*
This method gets the equation of the slope of a line
*/
double slope(Point p1, Point p2) {
	return (p2.y - p1.y) / (p2.x - p1.x);
}

/*
This method returns the y-intercept for the mx + b equation
*/
double findBase(Point p1, Point p2) {
	return (p2.x * p1.y) - (p1.x * p2.y);
}



void initialPointsDraw() {

	if (global.mouseClick == false) {
		initPoints();
	}
	int start = getStartingPoint();
	drawInitialPoints(global.pointArray, start);
	glutPostRedisplay();
}


/*
This method sets the user mode to custome mouse points
*/
void mousePoints() {

	global.mouseClick = true;  // change the user mode
	global.pointsCounter = 0;  // set the random point counter to 0
	global.pointArray.clear(); // empty the vector of points

}

/*
This method sets the user mode to random mouse points and increases the points counter every time it is selected.
random points begins with 100 point and increases by 100 every time it is selected.
*/
void randomPoints() {

	global.mouseClick = false;
	global.pointsCounter = global.pointsCounter + 100;
	initialPointsDraw();

}

/*
In this method MOUSE CLICKS are registered into variables
*/
void mouse(int btn, int state, int x, int y) {

	bool alreadyAPoint = false; // innocent until proven guilty

	if (global.mouseClick == true) {
		int h;
		h = glutGet(GLUT_WINDOW_HEIGHT);
		y = h - y;   /* In Glut, Y coordinate increases from top to bottom */

		// go through all the points that exist and check if this new point  is the same
		for (size_t i = 0; i < global.pointArray.size(); i++) {

			// get next point
			Point* pt = global.pointArray[i];
			// get point components
			int x2 = pt->x;
			int y2 = pt->y;

			// check if points are duplicates
			if (x2 == x && y2 == y) {
				alreadyAPoint = true;
			}

			// check that the points dont overlap
			if ((x2 > x - 10 && x2 < x + 10) && (y2 > y - 10 && y2 < y + 10)) {
				alreadyAPoint = true;
			}

		}

		// if the point does not exist, add it to the vector
		if (alreadyAPoint == false) {

			// This registers when the mouse is clicked and saves this to a boolean
			if (state == GLUT_DOWN) {
				global.x = x;	// save the click coordinates
				global.y = y;
			}

			// make sure you dont click outside the image
			if (global.x > global.w) {
				global.x = global.w;
			}
			if (global.y > global.h) {
				global.y = global.h;
			}

			// add the point to the vector
			global.pointArray.push_back(new Point(x, y));

		}

	}

	// draw the points
	drawInitialPoints(global.pointArray, 0);

}


/*
Glut keyboard function
*/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 0x1B:
	case'q':
	case 'Q':
		exit(0);
		break;
	case'm':
	case'M':
		// custom mouse clicks
		mousePoints();
		break;
	case'r':
	case'R':
		// random mouse clicks
		randomPoints();
		break;
	case 'c':
	case 'C':
		draw_polygon();
		break;
	case'f':
	case'F':
		initialPointsDraw();
		break;
	default:
		break;
	}
}//keyboard


 //Glut menu callback function
void menuFunc(int value)
{
	switch (value) {
	case MENU_QUIT:
		exit(0);
		break;
	case MENU_SAVE:
		// option
		break;
	case MENU_TRIANGLE:
		draw_polygon();
		break;
	case MENU_FILTER:
		// option
		break;
	}
}//menuFunc


void show_keys()
{
	printf("Q:quit\nF:blank\nC:convex hull + hull peel\nM:custom mouse points\n");
}

//Glut menu set up
void init_menu()
{
	int sub_menu = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Triangle", MENU_TRIANGLE);

	int main_menu = glutCreateMenu(&menuFunc);
	glutAddSubMenu("Modify", sub_menu);
	glutAddMenuEntry("Quit", MENU_QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*
This method initializes the data
*/
void init_data() {
	global.h = 750;
	global.w = 750;

	global.mouseClick = false;
}


int main(int argc, char** argv)
{

	//global.data = read_img((char*)FILENAME, &global.w, &global.h);
	/*if (global.data == NULL)
	{
		printf("Error loading image file %s\n", (char*)FILENAME);
		return 1;
	}*/
	init_data();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

	glutInitWindowSize(global.w, global.h);
	glutCreateWindow("Convex Hull");
	glShadeModel(GL_SMOOTH);
	glutDisplayFunc(display_image);
	//glutIdleFunc(display_image);
	glutMouseFunc(mouse);			// mouse clicks
	glutKeyboardFunc(keyboard);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, global.w, 0, global.h, 0, 1);
	init_menu();
	show_keys();

	glutMainLoop();

	return 0;
}


