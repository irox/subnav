
#include <GL/freeglut.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


static void reduceToUnit(float vec[3])
{

 float length;

 length = (float)sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
 if (length == 0.0f) length = 1.0;

 vec[0] /= length;
 vec[1] /= length;
 vec[2] /= length;
}

static void calcQuadNormal(float v[3][3], float out[3])
{
  float v1[3], v2[3];
  int x=0, y=1, z=2;

  v1[x] = v[0][x] - v[1][x];
  v1[y] = v[0][y] - v[1][y];
  v1[z] = v[0][z] - v[1][z];

  v2[x] = v[1][x] - v[2][x];
  v2[y] = v[1][y] - v[2][y];
  v2[z] = v[1][z] - v[2][z];

  out[x] = v1[y]*v2[z] - v1[z]*v2[y];
  out[y] = v1[z]*v2[x] - v1[x]*v2[z];
  out[z] = v1[x]*v2[y] - v1[y]*v2[x];

  reduceToUnit(out);
}

static void setVec(int index, float v[3][3], float vec[3]) {
  v[index][0] = vec[0];
  v[index][1] = vec[1];
  v[index][2] = vec[2];

}

static void setQuadNormal(float v1[3], float v2[3], float v3[3])
{
  float v[3][3];
  float res[3];

  setVec(0, v, v1);
  setVec(1, v, v2);
  setVec(2, v, v3);

  calcQuadNormal(v, res);

  glNormal3d(res[0], res[1], res[2]);
}

static void drawQuad(float v1[3], float v2[3], float v3[3], float v4[3])
{
    setQuadNormal(v1,v2,v3);
    glVertex3d(v1[0], v1[1], v1[2]);
    glVertex3d(v2[0], v2[1], v2[2]);
    glVertex3d(v3[0], v3[1], v3[2]);
    glVertex3d(v4[0], v4[1], v4[2]);
}

void drawSolidConicRingSegment(double irad, double orad, float distance, float width, int frontClosed, int backClosed, int slices)
{
  int x=0; int y=1; int z=2;
  GLfloat angle,/* x1, y1, x2, y2, x3, y3, x4, y4,*/ t1, t2;
/*  GLfloat x1i, y1i, x2i, y2i, x3i, y3i, x4i, y4i; */
  float o1[3];
  float o2[3];
  float o3[3];
  float o4[3];

  float i1[3];
  float i2[3];
  float i3[3];
  float i4[3];

  glBegin( GL_QUADS );
  glShadeModel(GL_SMOOTH);
  for( angle = 0.0f; angle < (2.0 * M_PI); angle += (M_PI/slices))
  {

    o1[x] = irad * sin(angle);
    o1[y] = irad * cos(angle);
    o1[z] = distance;

    o2[x] = orad * sin(angle);
    o2[y] = orad * cos(angle);
    o2[z] = width + distance;

    o3[x] = irad * sin(angle + M_PI / slices);
    o3[y] = irad * cos(angle + M_PI / slices);
    o3[z] = distance;

    o4[x] = orad * sin(angle + M_PI / slices);
    o4[y] = orad * cos(angle + M_PI / slices);
    o4[z] = width + distance;
    /* external quad. */
    drawQuad(o2,o4,o3,o1);
    /* internal quad. */
    t1 = irad * 0.15f;
    t2 = orad * 0.15f;
    i1[x] = (irad - t1) * sin(angle);
    i1[y] = (irad - t1) * cos(angle);
    i1[z] = distance;    

    i2[x] = (orad - t2) * sin(angle);
    i2[y] = (orad - t2) * cos(angle);
    i2[z] = width + distance;

    i3[x] = (irad - t1) * sin(angle + M_PI / slices);
    i3[y] = (irad - t1) * cos(angle + M_PI / slices);
    i3[z] = distance;

    i4[x] = (orad - t2) * sin(angle + M_PI / slices);
    i4[y] = (orad - t2) * cos(angle + M_PI / slices);
    i4[z] = width + distance;
    drawQuad(i1, i3, i4, i2);

    /* Side walls */
    if (backClosed == 1) {
      drawQuad(i4, o4, o2, i2);
    }
    if (frontClosed == 1) {
      drawQuad(i1, o1, o3, i3);
    }

  }
  glEnd();

}  

static void drawBox(float distance, float length, float height, float width1, float width2, float xOffset)
{
  float c[8][3];

  c[0][0] = width2 / 2 + xOffset;
  c[0][1] = height;
  c[0][2] = distance;

  c[1][0] = - width2 / 2 + xOffset;
  c[1][1] = height;
  c[1][2] = distance;

  c[2][0] = - width2 / 2 + xOffset;
  c[2][1] = height;
  c[2][2] = distance + length;
 
  c[3][0] = width2 / 2 + xOffset;
  c[3][1] = height;
  c[3][2] = distance + length; 

  c[4][0] = width1 / 2 + xOffset;
  c[4][1] = 0.0f;
  c[4][2] = distance;

  c[5][0] = - width1 / 2 + xOffset;
  c[5][1] = 0.0f;
  c[5][2] = distance;

  c[6][0] = - width1 / 2 + xOffset; 
  c[6][1] = 0.0f;
  c[6][2] = distance + length;

  c[7][0] = width1 / 2 + xOffset;
  c[7][1] = 0.0f;
  c[7][2] = distance + length;
  /* top */
  drawQuad(c[0], c[1], c[2], c[3]);
  /* back */
  drawQuad(c[6], c[7], c[3], c[2]);
  /* front */
  drawQuad(c[1], c[0], c[4], c[5]);
  /* Sides */
  drawQuad(c[0], c[3], c[7], c[4]);
  drawQuad(c[5], c[6], c[2], c[1]); /* Good right side */
}

static void drawCTower(float distance, float length, float height, float width) {
  drawBox(distance, length, height, width, width /2, 0);
}

static void drawSkids(float distance, float length, float height, float space, float thickness) {
  drawBox(distance, length, -height, thickness, thickness, space / 2);
  drawBox(distance, length, -height, thickness, thickness, - space / 2);

}

static void drawSolidStandardTail(float distance, float length, float width) {
  
  glBegin( GL_QUADS );
  drawCTower(distance, width, length, 0.02);  
  drawCTower(distance, width, -length, 0.02);
  glEnd();
  glRotated(90,0,0,1);

  glBegin( GL_QUADS );
  drawCTower(distance, width, length, 0.02);
  drawCTower(distance, width, -length, 0.02);
  glEnd();
}

/* 
static void drawSolidConicRing(void) {
  glRotated(90,1,0,0);
  drawSolidConicRingSegment(irad, orad, -1.0f, 2.0f, 0, 1);
}
*/

void drawSolidScaledSubmarineHull(double scale, float slices) {
 double framepointsXY[20][2] = {
            { 0.65, 0.75 },
            { 1, 0.977 },
            { 2, 1.567 },
            { 3, 1.93 },
            { 4, 2.098 },
            { 5, 2.215 },
            { 6, 2.284 },
            { 7, 2.307 },
            { 8, 2.303 },
            { 9, 2.292 },
            { 10, 2.262 },
            { 11, 2.186 },
            { 12, 2.086 },
            { 13, 1.894 },
            { 14, 1.683 },
            { 15, 1.414 },
            { 16, 1.095 },
            { 17, 0.767 },
            { 18, 0.381 },
            { 19, 0.04}};
  int i;
  int last = 18;

  glRotated(90,0,1,0);

  glShadeModel(GL_SMOOTH); 
  for (i=0; i < last+1; i++) {
    drawSolidConicRingSegment(
        framepointsXY[i][1] * scale,
        framepointsXY[i+1][1] * scale,
        framepointsXY[i][0] * scale,
        framepointsXY[i+1][0] * scale - framepointsXY[i][0] * scale,
        i==0?1:0,
        i==last?1:0,
        slices);
  }
  glBegin( GL_QUADS );
  drawCTower(6 * scale, 3 * scale, 4 * scale, 2 * scale);
  glEnd();

  glBegin( GL_QUADS );
  drawSkids(3 * scale, 11 * scale, 2.6 * scale, 1.75 * scale, 0.2 * scale);
  glEnd();

  drawSolidStandardTail(17.25* scale, 1.75 * scale, 1.25 * scale);
}

void drawWireConicRing(float irad, float orad, float slices)
{
  GLfloat angle, z, x1, y1, x2, y2, x3, y3, x4, y4, t1, t2;
  GLfloat x1i, y1i, x2i, y2i, x3i, y3i, x4i, y4i;

  for( angle = 0.0f; angle < (2.0 * M_PI); angle += (M_PI/slices))
  {
    x1 = irad * sin(angle);
    y1 = irad * cos(angle);
    x2 = orad * sin(angle);
    y2 = orad * cos(angle);
    x3 = irad * sin(angle + M_PI / slices);
    y3 = irad * cos(angle + M_PI / slices);
    x4 = orad * sin(angle + M_PI / slices);
    y4 = orad * cos(angle + M_PI / slices);
    z  = 0.40f;
    /* external quad. */
      glBegin( GL_LINE_LOOP );
    glVertex3d(x1, y1, 0.0f);
    glVertex3d(x3, y3, 0.0f);
    glVertex3d(x4, y4, z);
    glVertex3d(x2, y2, z);
    /* internal quad. */
    t1 = irad * 0.15f;
    t2 = orad * 0.15f;
    x1i = (irad - t1) * sin(angle);
    y1i = (irad - t1) * cos(angle);
    x2i = (orad - t2) * sin(angle);
    y2i = (orad - t2) * cos(angle);
    x3i = (irad - t1) * sin(angle + M_PI / slices);
    y3i = (irad - t1) * cos(angle + M_PI / slices);
    x4i = (orad - t2) * sin(angle + M_PI / slices);
    y4i = (orad - t2) * cos(angle + M_PI / slices);
    glVertex3d(x2i, y2i, z);
    glVertex3d(x4i, y4i, z);
    glVertex3d(x3i, y3i, 0.0f);
    glVertex3d(x1i, y1i, 0.0f);
    glEnd();
  }
}
/*
static void drawWireSubmarineHull(void) {
  drawWireConicRing();
}*/

