#pragma once

extern int n_control;
extern float pixSize;

extern float colCurve[];
extern float colDecast[];
extern float colPolyg[];
extern float colDeCpt[];
extern float colCtrlpt[];

void defaultCtrl(void);
int  mkDspWindow(void);
void drawGraphics(void);
void reshGraphics(int w, int h);
void clearGraphics(void);
void redrawAll(void);
