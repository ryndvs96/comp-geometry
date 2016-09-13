// 00_DefPoints.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
//#include <stdlib.h>
#include "GL/glut.h"
#include "Dwindow.h"
#include "GlobalState.h"

int Dsp_win;
int did_points = 0;

Status state;
// set up first and second products
std::unordered_set<int> prodLines;
std::unordered_set<int> prodCircles;

// stores all the points
std::map<int, Point3> thePoints;
int pointCounter = 0;
int refPoint = -1;

// stores the created lines
std::map<int, Line2> theLines;
int lineCounter = 0;
// stores values during creation
int linePoints[2];
int n_linePoints = 0;
int maxLinePoints = 2;

// stores the created circles
std::map<int, Circle> theCircles;
int circleCounter = 0;
// stores values during creation
int circlePoints[3];
int n_circlePoints = 0;
int maxCirclePoints = 3;

int main(int argc, char** argv) {  
	glutInit(&argc, argv);
	Dsp_win = mkDspWindow();
	glutMainLoop();
	return 0;
}

