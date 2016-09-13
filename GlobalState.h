#pragma once
#include <unordered_set>
#include <map>
extern int Dsp_win;

class Status {
public:
	enum {                  // input mode
		CLIK_POINT = 0,     // clicking to define
		MOVE_POINT = 1,     // click-and-drag
		CIRC_POINT = 2,		// select points to define circle
		LINE_POINT = 3,		// select points to define line

		FIRST_PROD = 4,		// select for first prod
		SECON_PROD = 5,		// select for secon prod
		REF_POINT  = 6,		// select for reference point
		ENTER_DENSITY = 7,	// enter the density for the grid
		CLEAR	   = 8		// clear the screen
	};
	enum {                  // some drawing modes
		DRAW_POINTS = 1,
		DRAW_FRAME  = 4,
		DRAW_AXES   = 8,
		DRAW_ORIENT = 16,
		DRAW_MASK   = 0xffff
	};
	enum {                  // which algorithm to execute
		DO_NOTHING = 0,
		CONVEX_HULL
	};
	int   input_mode;       // what to do
	int   draw_mode;        // what to draw
	int   algorithm;        // which algorithm to execute
	Status() : input_mode(CLIK_POINT),
		       draw_mode(DRAW_POINTS),
			   algorithm(DO_NOTHING)
	{}
	const char * getLabel() {
		switch (input_mode) {
		case CLIK_POINT: return "Mode: Define point by mouse";
		case MOVE_POINT: return "Mode: Move object by mouse";
		case CIRC_POINT: return "Mode: Define circle by points";
		case LINE_POINT: return "Mode: Define line by points";
		case FIRST_PROD: return "Mode: Select Lines and Circles for First Product";
		case SECON_PROD: return "Mode: Select Lines and Circles for Second Product";
		case REF_POINT:  return "Mode: Select Reference Point for Products";
		default: return "Point Display";
		}
	}
};
class Line1 {
public:
	Line1() : x(0), y(0), z(1) {}
	Line1(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	float x, y, z;
};
class Line2 {
public:
	Line2() : a(-1), b(-1), product(0) {}
	Line2(int aa, int bb) : a(aa), b(bb), product(0) {}
	bool hasPoint(int i) {
		return i == a || i == b;
	}
	bool equals(Line2 line) {
		return line.a == a && line.b == b;
	}
	int a, b, product;
};
class Point3 {
public:
	Point3() : x(0), y(0), z(1) {}
	Point3(float xx, float yy) : x(xx), y(yy), z(1) {}
	Point3(float xx, float yy, float zz) : x(xx / zz), y(yy / zz), z(1) {}
	float x, y, z;
};
class Circle {
public:
	Circle() : a(-1), b(-1), c(-1), product(0) {}
	Circle(int aa, int bb, int cc) : a(aa), b(bb), c(cc), product(0) {}
	bool hasPoint(int i) {
		return i == a || i == b || i == c;
	}
	bool equals(Circle circ) {
		return circ.a == a && circ.b == b && circ.c == c;
	}
	int a, b, c, product;
};
class Circle2 {
public:
	Circle2() : x(0.0f), y(0.0f), radius(0.0f) {}
	Circle2(float xx, float yy, float radd) : x(xx), y(yy), radius(radd) {}
	float x, y, radius;
};

extern Status state;
extern std::unordered_set<int> prodLines;
extern std::unordered_set<int> prodCircles;

// stores all the points
extern std::map<int, Point3> thePoints;
extern int pointCounter;
extern int refPoint;

// stores the created lines
extern std::map<int, Line2> theLines;
extern int lineCounter;
// stores values during creation
extern int linePoints[];
extern int n_linePoints;
extern int maxLinePoints;

// stores the created circles
extern std::map<int, Circle> theCircles;
extern int circleCounter;
// stores values during creation
extern int circlePoints[];
extern int n_circlePoints;
extern int maxCirclePoints;