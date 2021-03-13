
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <malloc.h>
#include <iostream>
#include "Point.h";
#include <freeglut.h>
#include <vector>
#include <stack>
#include <deque>
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

bool isInStack(deque<Line*> stack, Line* line);



typedef struct {

	int w, h;
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
		// check if both points are the same
		if (x2 == x1 && y2 == y1) {
			return true; // this point already exists!
		}
	}
	return false; // all good, point does not exist
}

/*
This initializes a
*/
vector<Point*> initPoints(void) {

	vector<Point*> pointArray;

	int pointAmount = 50;
	srand(time(NULL));
	for (int i = 0; i < pointAmount; i++) {

		// generate random number between 100 and 400
		int min = 10;
		int max = global.h - 50;

		int randX = min + rand() % (max - min + 1);

		int randY = min + rand() % (max - min + 1);


		// check to see if point already exists
		if (doesPointExist(pointArray, randX, randY) == false) {
			// add point to vector
			pointArray.push_back(new Point(randX, randY));
		}
		else {
			// create new points

			randX = min + rand() % (max - min + 1);
			randY = min + rand() % (max - min + 1);
			// add point to vector
			pointArray.push_back(new Point(randX, randY));
			//TODO:: CHECK AGAIN
		}

	}


	/*pointArray.push_back(new Point(250, 400));
	pointArray.push_back(new Point(100, 250));
	pointArray.push_back(new Point(150, 100));
	pointArray.push_back(new Point(150, 100));
	pointArray.push_back(new Point(200, 200));
	pointArray.push_back(new Point(300, 250));
	pointArray.push_back(new Point(350, 50));
	pointArray.push_back(new Point(500, 150));
	pointArray.push_back(new Point(350, 350));
	pointArray.push_back(new Point(350, 150));*/

	return pointArray;
}
bool isInHull(Point* pt1, Point* pt2, vector<Line*> currentLine) {

	if (currentLine.size() == 0)
	{
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
/*
This method grabs the lowest point on the y-axis and chooses that as the starting point
*/
int getStartingPoint(vector<Point*> pointArray, vector<Line*>& currentLines) {

	int index = -1;
	int minY = 1000;
	// go through all points in vector
	for (size_t i = 0; i < pointArray.size(); i++)
	{
		Point* pt = pointArray[i];
		int x = pt->x;
		int y = pt->y;
		if (isInHull(pt, pt, currentLines)) {
			if (minY > y) {
				minY = y;
				index = i;
			}
		}
	}

	return index;
}

/*
This method creates a new line between the starting
point and the next point in the vector and uses the convex hull algorithm
to check whether the line is valid
*/
bool checkLine(Point* currentPoint, Point* nextPoint, vector<Point*> pointArray, deque<Line*>& stack) {
	Line* l = new Line(*currentPoint, *nextPoint);
	if (isInStack(stack, l)) {

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
		for (size_t i = 0; i != pointArray.size(); i++) {
			// get next point
			Point* pt = pointArray[i];
			// get point components
			int x = pt->x;
			int y = pt->y;

			// check if points being used
			if ((x == x1 && y == y1) || (x == x2 && y == y2)) {
				//skip
			}
			else {

				// convex hull: create imaginary line 'd' between two points
				// 0 = ax+by+c (our line where all points compared to are on the line)
				// d = ax+by+c 
				int d = ((a * x) + (b * y) + c) / sqrt((a * a) + (b * b));

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
		glPointSize(10);
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
vector<Line*> drawLines(deque<Line*>& lineStack, vector<Line*>& usedPoints)
{
	// go through stack
	for (int i = lineStack.size(); i > 0; i--) {


		// get line at top of stack
		Line* line = lineStack.front();
		usedPoints.push_back(line);
		lineStack.pop_front();

		// point 1
		int x1 = line->pt1.x;
		int y1 = line->pt1.y;
		// point 2
		int x2 = line->pt2.x;
		int y2 = line->pt2.y;
		//printf("x1: %d",x1);

		glPointSize(10);
		glBegin(GL_LINES);
		glColor3f(0, 1.0, 0);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
		glFlush();
		glEnd();
	}

	return usedPoints;

}

bool isInStack(deque<Line*> stack, Line* line) {
	if (stack.empty()) {
		return true;

	}
	else {
		for (Line* ele : stack) {
			if ((ele->pt1.x == line->pt1.x && ele->pt1.y == line->pt1.y) && (ele->pt2.x == line->pt2.x && ele->pt2.y == line->pt2.y)) {
				return false;
			}
		}
		return true;
	}

	return true;
}

void removePointsUsed(vector<Point*>& pointArray, vector<Line*>& usedPoints) {
	for (size_t i = 0; i < pointArray.size(); i++) {
		Point* currPoint = pointArray[i];
		for (size_t j = 0; j < usedPoints.size(); j++) {
			Point pt1 = usedPoints[j]->pt1;
			Point pt2 = usedPoints[j]->pt2;
			if ((pt1.x == currPoint->x && pt1.y == currPoint->y) || (pt2.x == currPoint->x && pt2.y == currPoint->y))
			{
				pointArray.erase(pointArray.begin() + i);
			}
		}
	}

}

deque<Line*> convexHull(vector<Point*>& pointArray, vector<Line*>& usedPoints) {
	stack<Line*> lineStack;
	deque<Line*> lineDeque;

	// get a starting point (lowest y value)

	// draw points in open gl from vector points


	// draw line from previous point to next point in vector
	for (size_t i = 0; i != pointArray.size() - 1; i++) {
		for (size_t j = i + 1; j != pointArray.size() - 1; j++) {
			// get next point
			Point* pt = pointArray[i];
			Point* pt2 = pointArray[j];

			if (checkLine(pt, pt2, pointArray, lineDeque)) {
				if (isInHull(pt, pt2, usedPoints)) {
					Line* l = new Line(*pt, *pt2);
					if (isInStack(lineDeque, l)) {
						lineStack.push(l);
						lineDeque.push_back(l);
						printf("x1: %d", pt->x);

					}

				}
			}

		}
	}
	return lineDeque;
}

/*
 This method draws a polygon using open GL
 */
void draw_polygon(void)
{
	vector<Line*> usedPoints;
	deque<Line*> lineStack;
	// get points
	vector<Point*> pointArray = initPoints();

	int start = getStartingPoint(pointArray, usedPoints);
	drawInitialPoints(pointArray, start);

	//while (pointArray.size() > 2) {
	start = getStartingPoint(pointArray, usedPoints);
	lineStack = convexHull(pointArray, usedPoints);
	usedPoints = drawLines(lineStack, usedPoints);
	removePointsUsed(pointArray, usedPoints);
	usedPoints.clear();
	glutPostRedisplay();

	/*start = getStartingPoint(pointArray, usedPoints);
	lineStack = convexHull(pointArray, usedPoints);
	usedPoints = drawLines(lineStack, usedPoints);
	removePointsUsed(pointArray, usedPoints);
	usedPoints.clear();
	glutPostRedisplay();

	start = getStartingPoint(pointArray, usedPoints);
	lineStack = convexHull(pointArray, usedPoints);
	usedPoints = drawLines(lineStack, usedPoints);
	removePointsUsed(pointArray, usedPoints);
	usedPoints.clear();
	glutPostRedisplay();*/

	//}
	// TODO: Put line in a new vector which will be used to check if there is any line left 
	// clear lineStack 

	//lineStack = convexHull(pointArray, usedPoints);
	//usedPoints = drawLines(lineStack, usedPoints);

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
	case's':
	case'S':
		// option
		break;
	case 't':
	case 'T':
		draw_polygon();
		break;
	case'f':
	case'F':
		// option
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
	printf("Q:quit\nF:blank\nT:triangle\nS:blank\n");
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

void init_data() {
	global.h = 750;
	global.w = 750;
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
	glutKeyboardFunc(keyboard);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, global.w, 0, global.h, 0, 1);
	init_menu();
	show_keys();

	glutMainLoop();

	return 0;
}
