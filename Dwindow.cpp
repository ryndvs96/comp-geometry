
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include "GL/glut.h"
#include "Dwindow.h"
#include "GlobalState.h"

#define PI			3.1415926535897932384626433832795
#define SENSITIVITY 10

int win_w, win_h, scale;

int pt_hit_index = -1;
int ln_hit_index = -1;
int cr_hit_index = -1;
int pt_select = -1;
int ln_select = -1;
int cr_select = -1;
Point3 ln_prev;
Point3 cr_prev;
float pixSize;
int density = 100;

int menu_root, menu_inpt, menu_prod, menu_draw;

void mouseClick(int button, int state, int x, int y);
void mouseMotion(int x, int y);
Circle2 circleToCircle2(Circle circ);
Line1 line2ToLine1(Line2 line2);
Point3 getOrigin(Point3 a, Point3 b, Point3 c);
Point3 intersect(Line1 a, Line1 b);
float sqr(float x);
void evalProducts(float x, float y, float * first, float * second);
void deleteAll();


// colors

float colScnPrd[] = {0.51f,0.96f,0.17f}; // green
float colFstPrd[] = {0.93f,0.00f,0.93f}; // magenta
float colRefPt[]  = {0.02f,0.93f,1.00f}; // teal
float colCircle[] = {0.00f,0.77f,0.80f}; // turqoise
float colLine[]   = {0.00f,0.77f,0.80f}; // turqoise
float colBackgr[] = {0,0,0};
float colCtrlpt[] = {0,1,0};
float colMouse[]  =	{1,1,1};
float colAxes[]   = {1,1,1};
float colFrame[]  = {1,1,1};

// depths

float depthCircle = -0.05f;
float depthLine	  = 0.00f;
float depthCtrlpt = 0.05f;
float depthAxes	  = -0.10f;
float depthFrame  = -0.10f;
float depthMouse  = 0.10f;

static void menu_inpt_action(int which){
	if (which == Status::ENTER_DENSITY) {
		printf("Current density: %d\n", density);
		density = 0;
		while (density < 10 || density > 1000) {
			printf("Enter a density between 10 and 1000:\n");
			scanf_s("%d", &density);
		}
		printf("New density is %d\n", density);
		redrawAll();
	}
	else {
		state.input_mode = which;
		glutSetWindowTitle(state.getLabel());
	}
}
static void menu_draw_action(int which) {
	state.draw_mode ^= which;
	redrawAll();
}
static void menu_root_action(int which) {
	if (which == Status::CLEAR) {
		deleteAll();
		redrawAll();
	}
}
static void makeInputMenu() {
	menu_inpt = glutCreateMenu(menu_inpt_action);
	glutAddMenuEntry("define point by mouse", Status::CLIK_POINT);
	glutAddMenuEntry("define line by points", Status::LINE_POINT);
	glutAddMenuEntry("define circle by points", Status::CIRC_POINT);
	glutAddMenuEntry("move object by mouse", Status::MOVE_POINT);
}
static void makeProductMenu() {
	menu_prod = glutCreateMenu(menu_inpt_action);
	glutAddMenuEntry("select for first product", Status::FIRST_PROD);
	glutAddMenuEntry("select for second product", Status::SECON_PROD);
	glutAddMenuEntry("define reference point", Status::REF_POINT);
	glutAddMenuEntry("change density", Status::ENTER_DENSITY);
}
static void makeDrawMenu() {
	menu_draw = glutCreateMenu(menu_draw_action);
	glutAddMenuEntry("draw orientation", Status::DRAW_ORIENT);
	glutAddMenuEntry("draw axes", Status::DRAW_AXES);
	glutAddMenuEntry("draw frame", Status::DRAW_FRAME);
}
static void makeRootMenu() {
	makeInputMenu();
	makeDrawMenu();
	makeProductMenu();
	menu_root = glutCreateMenu(menu_root_action);
	glutAddMenuEntry("clear", Status::CLEAR);
	glutAddSubMenu("input mode", menu_inpt);
	glutAddSubMenu("product mode", menu_prod);
	glutAddSubMenu("draw mode", menu_draw);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setDcursor(int state) {
	if (state == GLUT_ENTERED)
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

int mkDspWindow(void) {
	/* set up the graphics window */
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
	glutInitWindowSize(720, 720);                  // initial size
	glutInitWindowPosition(160, 20);               // initial position
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	Dsp_win = glutCreateWindow("Point Display");
	clearGraphics();
	glutDisplayFunc(redrawAll);
	glutReshapeFunc(reshGraphics);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutEntryFunc(setDcursor);
	makeRootMenu();
	return Dsp_win;
}
void clearGraphics(void) {
	glutSetWindow(Dsp_win);
	glClearColor(colBackgr[0],colBackgr[1],colBackgr[2],0);
	glClear(GL_COLOR_BUFFER_BIT);
	glShadeModel(GL_FLAT);
}

void reshGraphics(int w, int h) {
	win_w = w;
	win_h = h;
	glutSetWindow(Dsp_win);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h) {
		scale = w/2;
		pixSize = 2.0f / (float) scale;
		glOrtho(-1.0, 1.0, 
			-1.0*(GLfloat)h/(GLfloat)w,
			1.0*(GLfloat)h/(GLfloat)w, 
			-1.0, 1.0);
	} else {
		scale = h/2;
		pixSize = 2.0f / (float) scale;
		glOrtho(-1.0*(GLfloat)w/(GLfloat)h,
			1.0*(GLfloat)w/(GLfloat)h, 
			-1.0, 1.0, -1.0, 1.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	clearGraphics();
	redrawAll();
}
void drawAxes(void) {
	glColor3fv(colAxes);
	glBegin(GL_LINES);
	glVertex3f(-1, 0, depthAxes);
	glVertex3f( 1, 0, depthAxes);
	glVertex3f( 0,-1, depthAxes);
	glVertex3f( 0, 1, depthAxes);
	glEnd();
}
void drawFrame(void) {
	glColor3fv(colFrame);
	glBegin(GL_LINES);
	glVertex3f(-1,-1, depthFrame);
	glVertex3f(-1, 1, depthFrame);
	glVertex3f(-1, 1, depthFrame);
	glVertex3f( 1, 1, depthFrame);
	glVertex3f( 1, 1, depthFrame);
	glVertex3f( 1,-1, depthFrame);
	glVertex3f( 1,-1, depthFrame);
	glVertex3f(-1,-1, depthFrame);
	glEnd();
}
void drawPoints(void) {
	if (thePoints.size() > 0) {
		glutSetWindow(Dsp_win);
		glColor3fv(colCtrlpt);
		glPointSize(4.0);
		glBegin(GL_POINTS);
		for (std::map<int, Point3>::iterator it = thePoints.begin(); it != thePoints.end(); ++it) {
			if (it->first == refPoint) 
				glColor3fv(colRefPt);
			glVertex3f(it->second.x - pixSize, it->second.y + pixSize, depthCtrlpt);
			if (it->first == refPoint)
				glColor3fv(colCtrlpt);
		}
		glEnd();
	}
}

void drawCircles(void) {
	for (std::map<int, Circle>::iterator it = theCircles.begin(); it != theCircles.end(); ++it) {
		Circle temp = it->second;
		Circle2 circ = circleToCircle2(temp);

		if (temp.product == 1)
			glColor3fv(colFstPrd);
		else if (temp.product == 2)
			glColor3fv(colScnPrd);
		else
			glColor3fv(colLine);
		if (circ.radius < 60) {
			glBegin(GL_LINE_LOOP);
			for (float i = 0.0f; i < 2 * PI; i += (float) PI / 100)
				glVertex3f(cos(i) * circ.radius + circ.x, sin(i) * circ.radius + circ.y, depthCircle);
			glEnd();
		}
		else {
			Point3 a = thePoints[temp.a];
			Point3 b = thePoints[temp.b];
			glBegin(GL_LINES);
			Line1 line = Line1(a.y*b.z - a.z*b.y, a.z*b.x - b.z*a.x, a.x*b.y - a.y*b.x);
			Point3 left = intersect(line, Line1(1, 0, 100));
			Point3 right = intersect(line, Line1(1, 0, -100));
			glVertex3f(left.x, left.y, depthCircle);
			glVertex3f(right.x, right.y, depthCircle);
			glEnd();
		}
	}
}

void drawLines(void) {
	for (std::map<int, Line2>::iterator it = theLines.begin(); it != theLines.end(); ++it) {
		Line1 line = line2ToLine1(it->second);
		Point3 left = intersect(line, Line1(1, 0, 100));
		Point3 right = intersect(line, Line1(1, 0, -100));
		Point3 top = intersect(line, Line1(0, 1, -100));
		Point3 bottom = intersect(line, Line1(0, 1, 100));

		if (it->second.product == 1)
			glColor3fv(colFstPrd);
		else if (it->second.product == 2)
			glColor3fv(colScnPrd);
		else
			glColor3fv(colLine);
		glBegin(GL_LINES);
		if (abs(left.y) > 1000.0f || abs(right.y) > 1000.0f) {
			glVertex3f(top.x, top.y, depthLine);
			glVertex3f(bottom.x, bottom.y, depthLine);
		}
		else {
			glVertex3f(left.x, left.y, depthLine);
			glVertex3f(right.x, right.y, depthLine);
		}
		glEnd();		
	}
}

bool diff(float x, float y) {
	return (x < 0 && y >= 0) || (x >= 0 && y < 0);
}

void drawCurve(void) {
	if (refPoint >= 0 && (prodLines.size() > 0 || prodCircles.size() > 0)) {
		Point3 ref = thePoints[refPoint];
		float u, v;
		evalProducts(ref.x, ref.y, &u, &v);
		float s = u / v;
		//printf("s = %.4f\n", s);

		// store values in 2D array and iterate
		float spacing = (2.0f / (float) density);
		std::vector< std::vector<float> > grid(density + 1, std::vector<float>(density + 1, 0.0f));

		int i, j;	float x, y;
		for (i = 0, x = -1.0f; i <= density; i++, x += spacing) {
			for (j = 0, y = -1.0f; j <= density; j++, y += spacing) {
				float first, second;
				evalProducts(x, y, &first, &second);
				grid[i][j] = first - s * second;
			}
		}
		//printf("s = %.4f\n", s);

		glColor3fv(colRefPt);
		for (i = 0, x = -1.0f; i < density; i++, x += spacing) {
			for (j = 0, y = -1.0f; j < density; j++, y += spacing) {
				float left = x;
				float right = x + spacing;
				float bottom = y;
				float top = y + spacing;

				float bottomLeft = grid[i][j];
				float bottomRight = grid[i + 1][j];
				float topLeft = grid[i][j + 1];
				float topRight = grid[i + 1][j + 1];

				bool startSet = false;
				bool endSet = false;

				float startx, starty, endx, endy;
				startx = starty = endx = endy = -2.0f;
				if (diff(topLeft, topRight)) {
					if (!startSet) {
						starty = top;
						startx = left + abs((topLeft / (abs(topLeft) + abs(topRight)))) * spacing;
						startSet = true;
					}
					else if (!endSet) {
						endy = top;
						endx = left + abs((topLeft / (abs(topLeft) + abs(topRight)))) * spacing;
						endSet = true;
					}
				}
				if (diff(topLeft, bottomLeft)) {
					if (!startSet) {
						starty = bottom + abs((bottomLeft / (abs(topLeft) + abs(bottomLeft)))) * spacing;
						startx = left;
						startSet = true;
					}
					else if (!endSet) {
						endy = bottom + abs((bottomLeft / (abs(topLeft) + abs(bottomLeft)))) * spacing;
						endx = left;
						endSet = true;
					}
				}
				if (diff(bottomLeft, bottomRight)) {
					if (!startSet) {
						starty = bottom;
						startx = left + abs((bottomLeft / (abs(bottomLeft) + abs(bottomRight)))) * spacing;
						startSet = true;
					}
					else if (!endSet) {
						endy = bottom;
						endx = left + abs((bottomLeft / (abs(bottomLeft) + abs(bottomRight)))) * spacing;
						endSet = true;
					}
				}
				if (diff(bottomRight, topRight)) {
					if (!startSet) {
						starty = bottom + abs((bottomRight / (abs(topRight) + abs(bottomRight)))) * spacing;
						startx = right;
						startSet = true;
					}
					else if (!endSet) {
						endy = bottom + abs((bottomRight / (abs(topRight) + abs(bottomRight)))) * spacing;
						endx = right;
						endSet = true;
					}
				}
				if (startSet && endSet) {
					glBegin(GL_LINES);
					glVertex3f(startx, starty, depthLine);
					glVertex3f(endx, endy, depthLine);
					glEnd();
				}
			}
		}
	}
}

int evalForCircles(Point3 p) {
	int val = 1;
	for (std::map<int, Circle>::iterator it = theCircles.begin(); it != theCircles.end(); ++it) {
		Circle circ = it->second;
		Circle2 circ2 = circleToCircle2(circ);
		Point3 a = thePoints[circ.a];
		Point3 b = thePoints[circ.b];
		Point3 c = thePoints[circ.c];
		double evaluate = pow(p.x - circ2.x, 2) + pow(p.y - circ2.y, 2) - pow(circ2.radius, 2);
		float u[] = { b.x - a.x, b.y - a.y };
		float v[] = { c.x - b.x, c.y - b.y };
		float wc = u[1] * v[0] - u[0] * v[1];
		if (wc < 0)
			evaluate *= -1;
		else if (wc == 0)
			evaluate = 0;

		if (evaluate < 0)
			val *= -1;
	}
	return val;
}

int evalForLines(Point3 p) {
	int val = 1;
	for (std::map<int, Line2>::iterator it = theLines.begin(); it != theLines.end(); ++it) {
		Line1 line = line2ToLine1(it->second);
		double dotprod = line.x * p.x + line.y * p.y + line.z * p.z;

		if (dotprod < 0)
			val *= -1;
	}
	return val;
}

void drawOrient(void) {
	float yellow[]	= { 1, 1, 0 };
	float red[]		= { 1, 0, 0 };
	glPointSize(2.0);
	glBegin(GL_POINTS);
	float spacing = (2.0f / (float)density);
	for (float i = -1.0; i <= 1.0; i += spacing) {
		for (float j = -1.0; j <= 1.0; j += spacing) {
			Point3 p = Point3(i, j, 1);
			int val = evalForLines(p) * evalForCircles(p);

			if (val > 0)
				glColor3fv(yellow);
			else if (val < 0)
				glColor3fv(red);

			glVertex3f((GLfloat) i, (GLfloat) j, depthCtrlpt);
		}
	}
	glEnd();
}

void redrawAll(void) {
	int m(0);
	glutSetWindow(Dsp_win);
	clearGraphics();
	if (state.draw_mode & Status::DRAW_ORIENT)
		drawOrient();
	if (state.draw_mode & Status::DRAW_AXES)
		drawAxes();
	if (state.draw_mode & Status::DRAW_FRAME)
		drawFrame();
	drawPoints();
	drawLines();
	drawCircles();
	drawCurve();
	glutSwapBuffers();
}

/*
 * deleting points from the grid
 */

void deleteLineByPoint(int i) {
	for (std::map<int, Line2>::iterator it = theLines.begin(); it != theLines.end(); ) {
		int key = it->first;
		Line2 line2 = it->second;
		++it;
		if (line2.hasPoint(i))
			theLines.erase(key);
	}
	if (n_linePoints > 0 && linePoints[0] == i)
		n_linePoints--;
}
void deleteCircleByPoint(int i) {
	for (std::map<int, Circle>::iterator it = theCircles.begin(); it != theCircles.end(); ) {
		int key = it->first;
		Circle circ = it->second;
		++it;
		if (circ.hasPoint(i)) 
			theCircles.erase(key);
	}
	bool swapping = false;
	for (int j = 0; j < n_circlePoints; j++) 
		if (!swapping && circlePoints[j] == i) 
			swapping = true;
		else if (swapping) 
			circlePoints[j - 1] = circlePoints[j];
	if (swapping)
		n_circlePoints--;
}
void deletePoint(int i) {
	deleteLineByPoint(i);
	deleteCircleByPoint(i);
	if (refPoint == i)
		refPoint = -1;
	thePoints.erase(i);
}
void deleteAll() {
	thePoints.clear();
	theLines.clear();
	theCircles.clear();
	refPoint = -1;
	n_circlePoints = 0;
	n_linePoints = 0;
}

/*
 * movement handling for lines and circles
 */

void moveCircle(int circId, float xx, float yy) {
	Circle circ = theCircles[circId];
	thePoints[circ.a].x += xx;
	thePoints[circ.b].x += xx;
	thePoints[circ.c].x += xx;

	thePoints[circ.a].y += yy;
	thePoints[circ.b].y += yy;
	thePoints[circ.c].y += yy;
}
void moveLine(int lineId, float xx, float yy) {
	Line2 line = theLines[lineId];
}

/*
 * mouse handling
 */

static void screen2world(int x, int y, float* pt) {
	int wy= win_h - y;
	pt[0] = ((float) (x - win_w/2)) / scale;
	pt[1] = ((float) (wy - win_h/2)) / scale;
}

static void world2screen(Point3 pt, int& x, int& y) {
	x = ((int) (pt.x * scale)) + win_w/2;
	y = ((int) (pt.y * scale)) + win_h/2;
	y = win_h - y;
}

static int getHitID(int x, int y) {
	int cx, cy;
	for (std::map<int, Point3>::iterator it = thePoints.begin(); it != thePoints.end(); ++it) {
		world2screen(it->second, cx, cy);
		if ((cx = cx - x) < 0) cx = -cx;
		if (cx < SENSITIVITY) {
			if ((cy = cy - y) < 0) cy = -cy;
			if (cy < SENSITIVITY) {
				return it->first;
			}
		}
	}
	return -1;
}

static int getLineIndex(Point3 p, int x, int y) {
	float cxfloat, cyfloat;
	int cx1, cy1, cx2, cy2;
	for (std::map<int, Line2>::iterator it = theLines.begin(); it != theLines.end(); ++it) {
		Line1 line = line2ToLine1(it->second);
		// x = (-c - by) / a		// y = (-c - ax) / b
		cxfloat = ((0.0f - line.z) - (line.y * p.y)) / line.x;
		cyfloat = ((0.0f - line.z) - (line.x * p.x)) / line.y;
		Point3 p1(cxfloat, p.y);
		Point3 p2(p.x, cyfloat);
		world2screen(p1, cx1, cy1);
		world2screen(p2, cx2, cy2);

		if ((abs(cx1 - x) < SENSITIVITY && abs(cy1 - y) < SENSITIVITY)
			|| (abs(cx2 - x) < SENSITIVITY && abs(cy2 - y) < SENSITIVITY)) {
			return it->first;
		}
	}
	return -1;
}

static int getCircIndex(Point3 p, int x, int y) {
	float cxfloatpos, cyfloatpos, cxfloatneg, cyfloatneg;
	int cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
	for (std::map<int, Circle>::iterator it = theCircles.begin(); it != theCircles.end(); ++it) {
		Circle2 circ = circleToCircle2(it->second);
		// x = sqrt(r^2 - (y - y0)^2) + x0		// y = sqrt(r^2 - (x - x0)^2) + y0
		// find where the points should be on the circle for pos and neg sqroots
		cxfloatpos = sqrt(sqr(circ.radius) - sqr(p.y - circ.y)) + circ.x;
		cyfloatpos = sqrt(sqr(circ.radius) - sqr(p.x - circ.x)) + circ.y;
		cxfloatneg = (0.0f - sqrt(sqr(circ.radius) - sqr(p.y - circ.y))) + circ.x;
		cyfloatneg = (0.0f - sqrt(sqr(circ.radius) - sqr(p.x - circ.x))) + circ.y;

		// construct the four points
		Point3 p1(cxfloatpos, p.y);
		Point3 p2(p.x, cyfloatpos);
		Point3 p3(cxfloatneg, p.y);
		Point3 p4(p.x, cyfloatneg);
		
		// world 2 screen the four points
		world2screen(p1, cx1, cy1);
		world2screen(p2, cx2, cy2);
		world2screen(p3, cx3, cy3);
		world2screen(p4, cx4, cy4);

		if ((abs(cx1 - x) < SENSITIVITY && abs(cy1 - y) < SENSITIVITY)
			|| (abs(cx2 - x) < SENSITIVITY && abs(cy2 - y) < SENSITIVITY)
			|| (abs(cx3 - x) < SENSITIVITY && abs(cy3 - y) < SENSITIVITY)
			|| (abs(cx4 - x) < SENSITIVITY && abs(cy4 - y) < SENSITIVITY)) {
			return it->first;
		}
	}
	return -1;
}

void mouseClick(int button, int gl_state, int x, int y) {
	float pt[2];
	int mod = glutGetModifiers();

	//
	// point definition requires acting on mouse up
	//
	screen2world(x,y,pt);
	if (state.input_mode == Status::CLIK_POINT || state.input_mode == Status::REF_POINT) {
		if (button == GLUT_LEFT_BUTTON) {
			if (gl_state == GLUT_UP) {
				int index = getHitID(x, y);
				if (state.input_mode == Status::CLIK_POINT && mod == GLUT_ACTIVE_SHIFT) {
					if (index >= 0)
						deletePoint(index);
				}
				if (state.input_mode == Status::REF_POINT && mod == GLUT_ACTIVE_SHIFT) {
					refPoint = -1;
				}
				else {
					if (state.input_mode == Status::REF_POINT && index >= 0) 
						refPoint = index;
					else {
						Point3 p(pt[0], pt[1]);
						int key = pointCounter;
						thePoints.insert(std::pair<int, Point3>(key, p));
						if (state.input_mode == Status::REF_POINT)
							refPoint = key;
						pointCounter++;
					}
				}
				clearGraphics();
				redrawAll();
			}
		}
		return;
	}

	//
	// control point motion requires capturing the control point
	//
	if (state.input_mode == Status::MOVE_POINT) {
		if (button == GLUT_LEFT_BUTTON) {
			if (gl_state == GLUT_DOWN) {      // match and highlight the hit
				pt_hit_index = getHitID(x, y);
				if (pt_hit_index > -1) {
					ln_hit_index = -1;
					cr_hit_index = -1;
					goto PointHit;
				}

				Point3 p(pt[0], pt[1]);
				ln_hit_index = getLineIndex(p, x, y);
				if (ln_hit_index > -1) {
					pt_hit_index = -1;
					cr_hit_index = -1;
					ln_prev = p;
					goto LineHit;
				}

				cr_hit_index = getCircIndex(p, x, y);
				if (cr_hit_index > -1) {
					ln_hit_index = -1;
					pt_hit_index = -1;
					cr_prev = p;
					goto CircHit;
				}

				pt_hit_index = -1;                 // nothing was hit
				ln_hit_index = -1;
				cr_hit_index = -1;
				return;
			} else {                            // move point and redisplay
				// completes motion at last position
				if (pt_hit_index > -1) {
					thePoints[pt_hit_index].x = pt[0];
					thePoints[pt_hit_index].y = pt[1];
					clearGraphics();
					redrawAll();
					return;
				}
				else if (ln_hit_index > -1) {
					Line2 line = theLines[ln_hit_index];
					float xdiff = pt[0] - ln_prev.x;
					float ydiff = pt[1] - ln_prev.y;

					thePoints[line.a].x += xdiff;
					thePoints[line.b].x += xdiff;
					thePoints[line.a].y += ydiff;
					thePoints[line.b].y += ydiff;

					clearGraphics();
					redrawAll();
					return;
				}
				else if (cr_hit_index > -1) {
					Circle circ = theCircles[cr_hit_index];
					float xdiff = pt[0] - cr_prev.x;
					float ydiff = pt[1] - cr_prev.y;

					thePoints[circ.a].x += xdiff;
					thePoints[circ.b].x += xdiff;
					thePoints[circ.c].x += xdiff;
					thePoints[circ.a].y += ydiff;
					thePoints[circ.b].y += ydiff;
					thePoints[circ.c].y += ydiff;

					clearGraphics();
					redrawAll();
					return;
				}
				else {
					return;
				}
			}
		}
		return;
	}

	//
	// define circle by clicking points
	//
	if (state.input_mode == Status::CIRC_POINT) {
		if (button == GLUT_LEFT_BUTTON) {
			if (gl_state == GLUT_DOWN) {
				if (mod == GLUT_ACTIVE_SHIFT) {
					Point3 p(pt[0], pt[1]);
					int index = getCircIndex(p, x, y);
					if (index >= 0)
						theCircles.erase(index);
				}
				else {
					if (n_circlePoints < maxCirclePoints) {
						int index = getHitID(x, y);
						if (index < 0) {
							Point3 p(pt[0], pt[1]);
							int key = pointCounter;
							thePoints.insert(std::pair<int, Point3>(key, p));
							pointCounter++;
							circlePoints[n_circlePoints] = key;
							n_circlePoints++;
						}
						else {
							bool check = true; // don't re-add points
							for (int j = 0; j < n_circlePoints; j++)
								if (circlePoints[j] == index)
									check = false;
							if (check) {
								circlePoints[n_circlePoints] = index;
								n_circlePoints++;
							}
						}
					}
					if (n_circlePoints == maxCirclePoints) {
						bool check = true;
						Circle circ(circlePoints[0], circlePoints[1], circlePoints[2]);
						for (std::map<int, Circle>::iterator it = theCircles.begin(); it != theCircles.end(); ++it) 
							if (it->second.equals(circ))
								check = false;
						if (check) 
							theCircles.insert(std::pair<int, Circle>(circleCounter++, circ));
						n_circlePoints = 0;
					}
				}
				clearGraphics();
				redrawAll();
			}
		}
		return;
	}

	//
	// define line by clicking points
	//
	if (state.input_mode == Status::LINE_POINT) {
		if (button == GLUT_LEFT_BUTTON) {
			if (gl_state == GLUT_DOWN) {
				if (mod == GLUT_ACTIVE_SHIFT) {
					Point3 p(pt[0], pt[1]);
					int index = getLineIndex(p, x, y);
					if (index >= 0)
						theLines.erase(index);
				}
				else {
					if (n_linePoints < maxLinePoints) {
						int index = getHitID(x, y);
						if (index < 0) {
							Point3 p(pt[0], pt[1]);
							int key = pointCounter;
							thePoints.insert(std::pair<int, Point3>(key, p));
							pointCounter++;
							linePoints[n_linePoints] = key;
							n_linePoints++;
						}
						else {
							bool check = true; // don't re-add points
							for (int j = 0; j < n_linePoints; j++)
								if (linePoints[j] == index)
									check = false;
							if (check) {
								linePoints[n_linePoints] = index;
								n_linePoints++;
							}
						}
					}
					if (n_linePoints == maxLinePoints) {
						bool check = true;
						Line2 line(linePoints[0], linePoints[1]);
						for (std::map<int, Line2>::iterator it = theLines.begin(); it != theLines.end(); ++it) 
							if (it->second.equals(line))
								check = false;
						if (check)
							theLines.insert(std::pair<int, Line2>(lineCounter++, line));
						n_linePoints = 0;
					}
				}
				clearGraphics();
				redrawAll();
			}
		}
		return;
	}

	//
	//	define first product by circle or line
	//
	if (state.input_mode == Status::FIRST_PROD) {
		Point3 p(pt[0], pt[1]);
		int index = getLineIndex(p, x, y);
		if (index < 0) {
			index = getCircIndex(p, x, y);
			if (index > -1) {
				if (mod == GLUT_ACTIVE_SHIFT) {
					theCircles[index].product = 0;
					prodCircles.erase(index);
				}
				else {
					theCircles[index].product = 1;
					prodCircles.insert(index);
				}
			}
		}
		else {
			if (mod == GLUT_ACTIVE_SHIFT) {
				theLines[index].product = 0;
				prodLines.erase(index);
			}
			else {
				theLines[index].product = 1;
				prodLines.insert(index);
			}
		}
		return;
	}

	//
	//	define second product by circle or line
	//
	if (state.input_mode == Status::SECON_PROD) {
		Point3 p(pt[0], pt[1]);
		int index = getLineIndex(p, x, y);
		if (index < 0) {
			index = getCircIndex(p, x, y);
			if (index > -1) {
				if (mod == GLUT_ACTIVE_SHIFT) {
					theCircles[index].product = 0;
					prodCircles.erase(index);
				}
				else {
					theCircles[index].product = 2;
					prodCircles.insert(index);
				}
			}
		}
		else {
			if (mod == GLUT_ACTIVE_SHIFT) {
				theLines[index].product = 0;
				prodLines.erase(index);
			}
			else {
				theLines[index].product = 2;
				prodLines.insert(index);
			}
		}
		return;
	}

	// point found, take note which one it is:
	PointHit:
	pt_select = pt_hit_index;
	return;
	// Line found, take note which one it is:
	LineHit:
	ln_select = ln_hit_index;
	return;
	// Control point found, take note which one it is:
	CircHit:
	cr_select = cr_hit_index;
	return;
}

void mouseMotion(int x, int y) {
	float pt[2];
  
	//
	//  moving a control point
	//
	if (state.input_mode == Status::MOVE_POINT) { // accept control point move
		screen2world(x, y, pt);
		if (pt_hit_index > -1) {
			thePoints[pt_hit_index].x = pt[0];             // change control point
			thePoints[pt_hit_index].y = pt[1];
		}
		else if (ln_hit_index > -1) {
			Line2 line = theLines[ln_hit_index];
			float xdiff = pt[0] - ln_prev.x;
			float ydiff = pt[1] - ln_prev.y;

			thePoints[line.a].x += xdiff;
			thePoints[line.b].x += xdiff;
			thePoints[line.a].y += ydiff;
			thePoints[line.b].y += ydiff;

			ln_prev = Point3(pt[0], pt[1]);
		}
		else if (cr_hit_index > -1) {
			Circle circ = theCircles[cr_hit_index];
			float xdiff = pt[0] - cr_prev.x;
			float ydiff = pt[1] - cr_prev.y;

			thePoints[circ.a].x += xdiff;
			thePoints[circ.b].x += xdiff;
			thePoints[circ.c].x += xdiff;
			thePoints[circ.a].y += ydiff;
			thePoints[circ.b].y += ydiff;
			thePoints[circ.c].y += ydiff;

			cr_prev = Point3(pt[0], pt[1]);
		}
		if (pt_hit_index > -1 || ln_hit_index > -1 || cr_hit_index > -1) {
			glColor3fv(colMouse);                     // redraw control point
			glBegin(GL_POINTS);
			glVertex3f(pt[0], pt[1], depthMouse);
			glEnd();
			clearGraphics();
			redrawAll();
		}
	}
}

// helper methods for computing the curve
void evalProducts(float x, float y, float * first, float * second) {
	if (refPoint < 0)
		return;

	*first = 1.0f;
	*second = 1.0f;
	bool hasFirst = false;
	bool hasSecon = false;

	// evaluate lines: ax + by + c = ?
	for (std::unordered_set<int>::iterator it = prodLines.begin(); it != prodLines.end(); ++it) {
		Line2 line2 = theLines[*it];
		if (line2.product > 0) {
			Line1 line1 = line2ToLine1(line2);
			float res = line1.x * x + line1.y * y + line1.z;
			if (line2.product == 1) {
				if (!hasFirst) {
					*first = 1.0f;
					hasFirst = true;
				}
				*first *= res;
			}
			else if (line2.product == 2) {
				if (!hasSecon) {
					*second = 1.0f;
					hasSecon = true;
				}
				*second *= res;
			}
		}
	}

	// evaluate circles: (x - x0)^2 + (y - y0)^2 - r^2 = ?
	for (std::unordered_set<int>::iterator it = prodCircles.begin(); it != prodCircles.end(); ++it) {
		Circle circ1 = theCircles[*it];
		if (circ1.product > 0) {
			Circle2 circ2 = circleToCircle2(circ1);
			float res = sqr(x - circ2.x) + sqr(y - circ2.y) - sqr(circ2.radius);
			if (circ1.product == 1) {
				if (!hasFirst) {
					*first = 1.0f;
					hasFirst = true;
				}
				*first *= res;
			}
			else if (circ1.product == 2) {
				if (!hasSecon) {
					*second = 1.0f;
					hasSecon = true;
				}
				*second *= res;
			}
		}
	}
}


// helper methods for conversion
float sqr(float x) {
	return x * x;
}

Point3 intersect(Line1 a, Line1 b) {
	return Point3(a.y*b.z - b.y*a.z, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

Point3 getOrigin(Point3 a, Point3 b, Point3 c) {
	Line1 ab = Line1(a.y*b.z - a.z*b.y, a.z*b.x - b.z*a.x, a.x*b.y - a.y*b.x);
	Line1 bc = Line1(b.y*c.z - b.z*c.y, b.z*c.x - c.z*b.x, b.x*c.y - b.y*c.x);

	Point3 midAB = Point3((a.x + b.x) / 2, (a.y + b.y) / 2);
	Point3 midBC = Point3((b.x + c.x) / 2, (b.y + c.y) / 2);

	Line1 abBi = Line1(0 - ab.y, ab.x, midAB.x*ab.y - midAB.y*ab.x);
	Line1 bcBi = Line1(0 - bc.y, bc.x, midBC.x*bc.y - midBC.y*bc.x);

	return intersect(abBi, bcBi);
}

Line1 line2ToLine1(Line2 line2) {
	Point3 a = thePoints[line2.a];
	Point3 b = thePoints[line2.b];
	return Line1(a.y*b.z - a.z*b.y, a.z*b.x - b.z*a.x, a.x*b.y - a.y*b.x);
}

Circle2 circleToCircle2(Circle circ) {
	Point3 a = thePoints[circ.a];
	Point3 b = thePoints[circ.b];
	Point3 c = thePoints[circ.c];
	Point3 o = getOrigin(a, b, c);
	float radius = sqrt(pow(a.x - o.x, 2) + pow(a.y - o.y, 2));
	return Circle2(o.x, o.y, radius);
}