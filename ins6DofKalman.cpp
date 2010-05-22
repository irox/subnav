#include "ins6DofKalman.h"
#include "ins3DCalcs.h"
#include <cmath>
#include <iostream>

using namespace std;

SixDofEKF::SixDofEKF() {
  setDim(15, 1, 6, 6, 6);
  Matrix qqQ(15, 15);
  Q=qqQ;
  period =  0.1; // 0.144; // 1.0 / 5.0;
  count = 0;
}

float _adjustRoll(float rawRoll) {
  if (rawRoll > ( 90.0 / 57.29578 )) {
    return -( (180.0 / 57.29578) - sqrt(rawRoll*rawRoll));
  } else if (rawRoll < (-90.0 / 57.29578)) {
    return ((180.0 / 57.29578) - rawRoll);
  }

  return rawRoll;
}


void SixDofEKF::makeBaseA() {
  int j = 0;
  float ar = _adjustRoll(x(12));

  float g = x(12); // Roll
  float b = x(10); // Pitch
  float a = x(11); // Heading

  float ax = x(13);
  float ay = x(14);
  float az = x(15);

  A(1, 1) = 0.0;
  A(1, 2) = period;
  A(1, 3) = 0.5 * period * period;
  A(1, 4) = 0.0;  A(1, 5) = 0.0;  A(1, 6) = 0.0;
  A(1, 7) = 0.0;  A(1, 8) = 0.0;  A(1, 9) = 0.0;
  A(1, 10) = 0.0; A(1, 11) = 0.0; A(1, 12) = 0.0;
  A(1, 13) = 0.0; A(1, 14) = 0.0; A(1, 15) = 0.0;


  A(2, 1) = 0.0;
  A(2, 2) = 0.0;
  A(2, 3) = period;
  A(2, 4) = 0.0;  A(2, 5) = 0.0;  A(2, 6) = 0.0;
  A(2, 7) = 0.0;  A(2, 8) = 0.0;  A(2, 9) = 0.0;
  A(2, 10) = 0.0; A(2, 11) = 0.0; A(2, 12) = 0.0;
  A(2, 13) = 0.0; A(2, 14) = 0.0; A(2, 15) = 0.0;

  A(3, 1) = 0.0;
  A(3, 2) = 0.0;
  A(3, 3) = 0.0;
  A(3, 4) = 0.0;  A(3, 5) = 0.0;  A(3, 6) = 0.0;
  A(3, 7) = 0.0;  A(3, 8) = 0.0;  A(3, 9) = 0.0;
  A(3, 10) = 0.0; A(3, 11) = 0.0; A(3, 12) = 0.0;
  A(3, 13) = 0.0; A(3, 14) = 0.0; A(3, 15) = 0.0;

  A(4, 1) = 0.0;  A(4, 2) = 0.0;  A(4, 3) = 0.0;
  A(4, 4) = 1.0;
  A(4, 5) = period;
  A(4, 6) = 0.5 * period * period;
  A(4, 7) = 0.0;  A(4, 8) = 0.0;  A(4, 9) = 0.0;
  A(4, 10) = 0.0; A(4, 11) = 0.0; A(4, 12) = 0.0;
  A(4, 13) = 0.0; A(4, 14) = 0.0; A(4, 15) = 0.0;

  A(5, 1) = 0.0;  A(5, 2) = 0.0;  A(5, 3) = 0.0;
  A(5, 1) = 0.0;
  A(5, 2) = 1.0;
  A(5, 3) = period;
  A(5, 7) = 0.0;  A(5, 8) = 0.0;  A(5, 9) = 0.0;
  A(5, 10) = 0.0; A(5, 11) = 0.0; A(5, 12) = 0.0;
  A(5, 13) = 0.0; A(5, 14) = 0.0; A(5, 15) = 0.0;

  A(6, 1) = 0.0;  A(6, 2) = 0.0;  A(6, 3) = 0.0;
  A(6, 4) = 0.0;
  A(6, 5) = 0.0;
  A(6, 6) = 1.0;
  A(6, 7) = 0.0;  A(6, 8) = 0.0;  A(6, 9) = 0.0;
  A(6, 10) = 0.0; A(6, 11) = 0.0; A(6, 12) = 0.0;
  A(6, 13) = 0.0; A(6, 14) = 0.0; A(6, 15) = 0.0;

  A(7, 1) = 0.0;  A(7, 2) = 0.0;  A(7, 3) = 0.0;
  A(7, 4) = 0.0;  A(7, 5) = 0.0;  A(7, 6) = 0.0;
  A(7, 7) = 1.0;
  A(7, 8) = period;
  A(7, 9) = 0.5 * period * period;
  A(7, 10) = 0.0; A(7, 11) = 0.0; A(7, 12) = 0.0;
  A(7, 13) = 0.0; A(7, 14) = 0.0; A(7, 15) = 0.0;

  j = 8;
  A(j, 1) = 0.0;  A(j, 2) = 0.0;  A(j, 3) = 0.0;
  A(j, 4) = 0.0;  A(j, 5) = 0.0;  A(j, 6) = 0.0;
  A(j, 5) = 0.0;
  A(j, 6) = 1.0;
  A(j, 7) = period;
  A(j, 10) = 0.0; A(j, 11) = 0.0; A(j, 12) = 0.0;
  A(j, 13) = 0.0; A(j, 14) = 0.0; A(j, 15) = 0.0;

  j = 9;
  A(j, 1) = 0.0;  A(j, 2) = 0.0;  A(j, 3) = 0.0;
  A(j, 4) = 0.0;  A(j, 5) = 0.0;  A(j, 6) = 0.0;
  A(j, 7) = 0.0;
  A(j, 8) = 0.0;
  A(j, 9) = 1.0;
  A(j, 10) = 0.0; A(j, 11) = 0.0; A(j, 12) = 0.0;
  A(j, 13) = 0.0; A(j, 14) = 0.0; A(j, 15) = 0.0;

  j = 10;
  A(j, 1) = - x(13) * cos(x(11)) * sin(x(10)) + x(14) * cos(x(11)) * cos(x(10)) * sin(x(12)) + x(15) * cos(x(11)) * cos(x(10)) * cos(x(12));
  A(j, 2) = 0.0;
  A(j, 3) = 0.0;
//  A(j, 4) = - x(13) * sin(x(11)) * sin(x(10)) + x(14) * sin(x(11)) * cos(x(10)) * sin(x(12));
  A(j, 5) = 0.0;
  A(j, 6) = 0.0;
//  A(j, 7) = - ax * cos(b) - ay * sin(b) * sin(g) - az * sin(b) * cos(g);
  A(j, 8) = 0.0;
  A(j, 9) = 0.0;
  A(j, 10) = 1.0;
  A(j, 11) = 0.0;
  A(j, 12) = 0.0;
  A(j, 13) = 0.0; A(j, 14) = 0.0; A(j, 15) = 0.0;

  j = 11;
//  A(j, 1) = - ax * sin(a) * cos(b) + ay * ( - sin(a) * sin(b) * sin(g) - cos(a) * cos(g) ) + az * ( -sin(a) * sin(b) * cos(g) + cos(a) * cos(g));
  A(j, 2) = 0.0;  
  A(j, 3) = 0.0;
//  A(j, 4) = ax * cos(a) * cos(b) + ay * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g) ) + az * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g));
  A(j, 5) = 0.0;
  A(j, 6) = 0.0;
  A(j, 7) = 0.0;
  A(j, 8) = 0.0;
  A(j, 9) = 0.0;
  A(j, 10) = 0.0; 
  A(j, 11) = 1.0; 
  A(j, 12) = 0.0;
  A(j, 13) = 0.0; A(j, 14) = 0.0; A(j, 15) = 0.0;

  j = 12;
//  A(j, 1) = ay * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g) ) + az * ( -cos(a) * sin(b) * sin(g) + sin(a) * cos(g) ) ; 
  A(j, 2) = 0.0;
  A(j, 3) = 0.0;
//  A(j, 4) = ay * ( sin(a) * sin(b) * cos(g) - cos(a) * sin(g)) + az * (- sin(a) * sin(b) * sin(g) - cos(a) * cos(g));
  A(j, 5) = 0.0;
  A(j, 6) = 0.0;
//  A(j, 7) = ay * cos(b) * cos(g) - az  * cos(b) * sin(g);
  A(j, 8) = 0.0;
  A(j, 9) = 0.0;
  A(j, 10) = 0.0;
  A(j, 11) = 0.0;
  A(j, 12) = 1.0;
  A(j, 13) = 0.0; A(j, 14) = 0.0; A(j, 15) = 0.0;

  j = 13;
  A(j, 1) = cos(a) * cos(b);
  A(j, 2) = 0.0;
  A(j, 3) = 0.0;
  A(j, 4) = sin(a) * cos(b);
  A(j, 5) = 0.0;
  A(j, 6) = 0.0;
  A(j, 7) = - sin(a);
  A(j, 8) = 0.0;
  A(j, 9) = 0.0;
  A(j, 10) = 0.0; A(j, 11) = 0.0; A(j, 12) = 0.0;
  A(j, 13) = 1.0;
  A(j, 14) = 0.0;
  A(j, 15) = 0.0;

  j = 14;
//  A(j, 1) = cos(a) * sin(b) * sin(g) - sin(a) * cos(g);
  A(j, 2) = 0.0;
  A(j, 3) = 0.0;
//  A(j, 4) = sin(a) * sin(b) * sin(g) + cos(a) * cos(g);
  A(j, 5) = 0.0;
  A(j, 6) = 0.0;
//  A(j, 7) = cos(b) * sin(g);
  A(j, 8) = 0.0;
  A(j, 9) = 0.0;
  A(j, 10) = 0.0; A(j, 11) = 0.0; A(j, 12) = 0.0;
  A(j, 13) = 0.0;
  A(j, 14) = 1.0;
  A(j, 15) = 0.0;

  j = 15;
//  A(j, 1) = cos(a) * sin(b) * cos(g) + sin(a) * sin(g);
  A(j, 2) = 0.0;
  A(j, 3) = 0.0;
//  A(j, 4) = sin(a) * sin(b) * cos(g) - cos(a) * sin(g);
  A(j, 5) = 0.0;
  A(j, 6) = 0.0;
//  A(j, 7) = cos(b) * cos(g);
  A(j, 8) = 0.0;
  A(j, 9) = 0.0;
//  A(j, 10) = 0.0;
  A(j, 11) = 0.0; A(j, 12) = 0.0;
  A(j, 13) = 0.0;
  A(j, 14) = 0.0;
  A(j, 15) = 1.0;

}


void SixDofEKF::makeA() {
  int j = 0;

  float g = x(12); // Roll
  float b = x(10); // Pitch
  float a = x(11); // Heading

  float ax = x(13);
  float ay = x(14);
  float az = x(15);

  A(1, 2) = period;
  A(1, 3) = 0.5 * period * period;

  A(2, 3) = period;

  A(4, 5) = period;
  A(4, 6) = 0.5 * period * period;

  A(5, 3) = period;

  A(7, 8) = period;
  A(7, 9) = 0.5 * period * period;

  j = 8;
  A(j, 7) = period;

/*
  j = 10;
  A(j, 1) = x(13) * cos(x(10)) * cos(x(11));
  A(j, 4) = x(13) * cos(x(10)) * sin(x(11));
  A(j, 7) = - x(13) * sin(x(10));
  
  j = 11;
  A(j, 1) = x(13) * sin(x(10)) * sin(x(11));
  A(j, 4) = x(13) * sin(x(10)) * cos(x(11));
  
  j = 13;
  A(j, 1) = sin(x(10)) * cos(x(11));
  A(j, 4) = sin(x(10)) * sin(x(11));
*/
  j = 10;
  A(j, 1) = - x(13) * cos(x(11)) * sin(x(10)) + x(14) * cos(x(11)) * cos(x(10)) * sin(x(12)) + x(15) * cos(x(11)) * cos(x(10)) * cos(x(12));
  A(j, 4) = - x(13) * sin(x(11)) * sin(x(10)) + x(14) * sin(x(11)) * cos(x(10)) * sin(x(12));
  A(j, 7) = - ax * cos(b) - ay * sin(b) * sin(g) - az * sin(b) * cos(g);
  A(j,13) = -((1 / (az*az + ay*ay + ax*ax)) - (2*ax*ax / (  (az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) )))/(sqrt(1 - (ax*ax/(  (az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) ))));
  A(j,14) = 2 * ax * ay / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) * sqrt(1 - (ax*ax / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax)))));
  A(j,15) = 2 * ax * az / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) * sqrt(1 - (ax*ax / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax)))));

  j = 11;
  A(j, 1) = - ax * sin(a) * cos(b) + ay * ( - sin(a) * sin(b) * sin(g) - cos(a) * cos(g) ) + az * ( -sin(a) * sin(b) * cos(g) + cos(a) * cos(g));
  A(j, 4) = ax * cos(a) * cos(b) + ay * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g) ) + az * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g));
 
  j = 12;
  A(j, 1) = ay * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g) ) + az * ( -cos(a) * sin(b) * sin(g) + sin(a) * cos(g) ) ;
  A(j, 4) = ay * ( sin(a) * sin(b) * cos(g) - cos(a) * sin(g)) + az * (- sin(a) * sin(b) * sin(g) - cos(a) * cos(g));
  A(j, 7) = ay * cos(b) * cos(g) - az  * cos(b) * sin(g);
  A(j,14) = (1 / (az*az + ay*ay) - (2 * ay * ay / ((az*az + ay*ay) * (az*az + ay*ay)))) / sqrt(1 - (ay * ay / ((az*az + ay*ay) * (az*az + ay*ay))));
  A(j,15) = 2 * ay * az / ((az*az + ay*ay)*(az*az + ay*ay) * sqrt(1 - (ay*ay / ((az*az + ay*ay)*(az*az + ay*ay)))));

  j = 13;
  A(j, 1) = cos(a) * cos(b);
  A(j, 4) = sin(a) * cos(b);
  A(j, 7) = - sin(a);

  j = 14;
  A(j, 1) = cos(a) * sin(b) * sin(g) - sin(a) * cos(g);
  A(j, 4) = sin(a) * sin(b) * sin(g) + cos(a) * cos(g);
  A(j, 7) = cos(b) * sin(g);

  j = 15;
  A(j, 1) = cos(a) * sin(b) * cos(g) + sin(a) * sin(g);
  A(j, 4) = sin(a) * sin(b) * cos(g) - cos(a) * sin(g);
  A(j, 7) = cos(b) * cos(g);

 // cout << A << endl;
}


void SixDofEKF::makeBaseW() {
  int j;

  j = 1;
  W(j,1) = 1.0;
  W(j,2) = 1.0;
  W(j,3) = 1.0;
  W(j,4) = 1.0;
  W(j,5) = 1.0;
  W(j,6) = 1.0;

  j = 2;
  W(j,1) = 0.0; W(j,2) = 0.0; W(j,3) = 0.0; W(j,4) = 0.0; W(j,5) = 0.0; W(j,6) = 0.0;

  j = 3;
  W(j,1) = 0.0; W(j,2) = 0.0; W(j,3) = 0.0; W(j,4) = 0.0; W(j,5) = 0.0; W(j,6) = 0.0;

  j = 4;
  W(j,1) = 1.0;
  W(j,2) = 1.0;
  W(j,3) = 1.0;
  W(j,4) = 1.0;
  W(j,5) = 1.0;
  W(j,6) = 1.0;

  j = 5;
  W(j,1) = 0.0; W(j,2) = 0.0; W(j,3) = 0.0; W(j,4) = 0.0; W(j,5) = 0.0; W(j,6) = 0.0;

  j = 6;
  W(j,1) = 0.0; W(j,2) = 0.0; W(j,3) = 0.0; W(j,4) = 0.0; W(j,5) = 0.0; W(j,6) = 0.0;

  j = 7;
  W(j,1) = 1.0;
  W(j,2) = 0.0;
  W(j,3) = 1.0;
  W(j,4) = 1.0;
  W(j,5) = 1.0;
  W(j,6) = 1.0;

  j = 8;
  W(j,1) = 0.0; W(j,2) = 0.0; W(j,3) = 0.0; W(j,4) = 0.0; W(j,5) = 0.0; W(j,6) = 0.0;

  j = 9;
  W(j,1) = 0.0; W(j,2) = 0.0; W(j,3) = 0.0; W(j,4) = 0.0; W(j,5) = 0.0; W(j,6) = 0.0;

  j =  10;
  W(j,1) = 1.0;
  W(j,2) = 0.0;
  W(j,3) = 0.0;
  W(j,4) = 01.0;
  W(j,5) = 01.0;
  W(j,6) = 01.0;

  j = 11;
  W(j,1) = 0.0;
  W(j,2) = 1.0;
  W(j,3) = 0.0;
  W(j,4) = 0.0;
  W(j,5) = 0.0;
  W(j,6) = 0.0;

  j = 12;
  W(j,1) = 0.0;
  W(j,2) = 0.0;
  W(j,3) = 1.0;
  W(j,4) = 0.0;
  W(j,5) = 01.0;
  W(j,6) = 01.0;

  j = 13;
  W(j,1) = 0.0;
  W(j,2) = 0.0;
  W(j,3) = 0.0;
  W(j,4) = 1.0;
  W(j,5) = 0.0;
  W(j,6) = 0.0;

  j = 14;
  W(j,1) = 0.0;
  W(j,2) = 0.0;
  W(j,3) = 0.0;
  W(j,4) = 0.0;
  W(j,5) = 1.0;
  W(j,6) = 0.0;

  j = 15;
  W(j,1) = 0.0;
  W(j,2) = 0.0;
  W(j,3) = 0.0;
  W(j,4) = 0.0;
  W(j,5) = 0.0;
  W(j,6) = 1.0;

}

void SixDofEKF::makeBaseQ() {
  int j;

  float sd1 = 0.06; // accel vector
  float sd1a = 0.002;
  float sd2 = sd1 * period; // velocity vector
  float sd3 = sd1 * period * period; // distance

  float sd4 = 0.0035; // / 57.3; // gyro
  float sd5 = 0.0035; // / 57.3; // compass
  // 15 x 15 diag matrix.

  j = 1;
  Q(j,1) = sd1;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 2;
  Q(j,1) = 0.0;
  Q(j,2) = sd2;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 3;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = sd3;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 4;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = sd1;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 5;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = sd2;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 6;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = sd3;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 7;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = sd1;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 8;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = sd2;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 9;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = sd3;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 10;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = sd4;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 11;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = sd5;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 12;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = sd4;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 13;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = sd1a;
  Q(j,14) = 0.0;
  Q(j,15) = 0.0;

  j = 14;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = sd1a;
  Q(j,15) = 0.0;

  j = 15;
  Q(j,1) = 0.0;
  Q(j,2) = 0.0;
  Q(j,3) = 0.0;
  Q(j,4) = 0.0;
  Q(j,5) = 0.0;
  Q(j,6) = 0.0;
  Q(j,7) = 0.0;
  Q(j,8) = 0.0;
  Q(j,9) = 0.0;
  Q(j,10) = 0.0;
  Q(j,11) = 0.0;
  Q(j,12) = 0.0;
  Q(j,13) = 0.0;
  Q(j,14) = 0.0;
  Q(j,15) = sd1a;
}

void SixDofEKF::makeQ() {
//  Q(1,1) = 0.01 * 0.01;
}

void SixDofEKF::makeBaseH() {
  int j;
  
  float sc1 = 1.0;
  float sc2 = 1.0;
  
  j = 1;
  H(j,1) = 0.0; H(j,2) = 0.0; H(j,3) = 0.0; H(j,4) = 0.0; H(j,5) = 0.0; H(j,6) = 0.0; H(j,7) = 0.0; H(j,8) = 0.0; H(j,9) = 0.0;  H(j,10) = 1;  H(j,11) = 0.0; H(j,12) = 0.0; H(j,13) = 0.0; H(j,14) = 0.0; H(j,14) = 0.0;

  j = 2;
  H(j,1) = 0.0; H(j,2) = 0.0; H(j,3) = 0.0; H(j,4) = 0.0; H(j,5) = 0.0; H(j,6) = 0.0; H(j,7) = 0.0; H(j,8) = 0.0; H(j,9) = 0.0; H(j,10) = 0.0; H(j,11) = sc1; H(j,12) = 0.0; H(j,13) = 0.0; H(j,14) = 0.0; H(j,14) = 0.0;


  j = 3;
  H(j,1) = 0.0; H(j,2) = 0.0; H(j,3) = 0.0; H(j,4) = 0.0; H(j,5) = 0.0; H(j,6) = 0.0; H(j,7) = 0.0; H(j,8) = 0.0; H(j,9) = 0.0; H(j,10) = 0.0; H(j,11) = 0.0;  H(j,12) = sc1;  H(j,13) = 0.0; H(j,14) = 0.0; H(j,15) = 0.0;


  j = 4;
  H(j,1) = 0.0; H(j,2) = 0.0; H(j,3) = 0.0; H(j,4) = 0.0; H(j,5) = 0.0; H(j,6) = 0.0; H(j,7) = 0.0; H(j,8) = 0.0; H(j,9) = 0.0; H(j,10) = 0.0; H(j,11) = 0.0; H(j,12) = 0.0; H(j,13) = sc2; H(j,14) = 0.0; H(j,15) = 0.0;


  j = 5;
  H(j,1) = 0.0; H(j,2) = 0.0; H(j,3) = 0.0; H(j,4) = 0.0; H(j,5) = 0.0; H(j,6) = 0.0; H(j,7) = 0.0; H(j,8) = 0.0; H(j,9) = 0.0; H(j,10) = 0.0; H(j,11) = 0.0; H(j,12) = 0.0; H(j,13) = 0.0; H(j,14) = sc2; H(j,15) = 0.0;


  j = 6;
  H(j,1) = 0.0; H(j,2) = 0.0; H(j,3) = 0.0; H(j,4) = 0.0; H(j,5) = 0.0; H(j,6) = 0.0; H(j,7) = 0.0; H(j,8) = 0.0; H(j,9) = 0.0; H(j,10) = 0.0; H(j,11) = 0.0; H(j,12) = 0.0; H(j,13) = 0.0; H(j,14) = 0.0; H(j,15) = sc2;


}

void SixDofEKF::makeH() {
  int j;

  int ax = x(13);
  int ay = x(14);
  int az = x(15);

  j = 1;
 // H(j,13) = -x(13) / sqrt( (1 + x(13) * x(13)) * (x(13) * x(13) + x(14) * x(14) + x(15) * x(15))); // for x(13)
  H(j,13) = -((1 / (az*az + ay*ay + ax*ax)) - (2*ax*ax / (  (az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) )))/(sqrt(1 - (ax*ax/(  (az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) ))));
  
  H(j,14) = 2 * ax * ay / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) * sqrt(1 - (ax*ax / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax)))));

  H(j,15) = 2 * ax * az / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax) * sqrt(1 - (ax*ax / ((az*az + ay*ay + ax*ax)*(az*az + ay*ay + ax*ax)))));

  j = 3;
  H(j,14) = (1 / (az*az + ay*ay) - (2 * ay * ay / ((az*az + ay*ay) * (az*az + ay*ay)))) / sqrt(1 - (ay * ay / ((az*az + ay*ay) * (az*az + ay*ay))));
  H(j,15) = 2 * ay * az / ((az*az + ay*ay)*(az*az + ay*ay) * sqrt(1 - (ay*ay / ((az*az + ay*ay)*(az*az + ay*ay)))));

  //H(j,12)  = - 1 / sqrt( x(14) * x(14) + x(15) * x(15) + ( x(14) * x(14) * x(14) - x(15) * x(15) * x(14) ) / sqrt(x(14) * x(14) + x(15) * x(15)));

//  H(j, 14) = -1 / sqrt(2 * x(14) - 2 * x(14) / sqrt( 2 * x(14))); // x(14)  = y
//  H(j, 15) = -x(14) / sqrt( 2 * x(15) - 2 * x(15) / sqrt( 2 * x(15)));

//  H(j,12) = - 1 / sqrt( 2 * x(14) - 2 * x(14) / sqrt(3 * x(14)));
}

void SixDofEKF::makeBaseV() {
  int j;

  j = 1;
  V(j,1) = 1.0;
  V(j,2) = 0.0;
  V(j,3) = 0.0;
  V(j,4) = 1.0;
  V(j,5) = 1.0;
  V(j,6) = 1.0;

  j = 2;
  V(j,1) = 0.0;
  V(j,2) = 1.0;
  V(j,3) = 0.0;
  V(j,4) = 0.0;
  V(j,5) = 0;
  V(j,6) = 0.0;

  j = 3;
  V(j,1) = 0.0;
  V(j,2) = 0.0;
  V(j,3) = 1.0;
  V(j,4) = 0.0;
  V(j,5) = 0.0;
  V(j,6) = 0.0;

  j = 4;
  V(j,1) = 0.0;
  V(j,2) = 0.0;
  V(j,3) = 0.0;
  V(j,4) = 1.0;
  V(j,5) = 0.0;
  V(j,6) = 0.0;

  j = 5;
  V(j,1) = 0.0;
  V(j,2) = 0.0;
  V(j,3) = 0.0;
  V(j,4) = 0.0;
  V(j,5) = 1.0;
  V(j,6) = 0.0;

  j = 6;
  V(j,1) = 0.0;
  V(j,2) = 0.0;
  V(j,3) = 0.0;
  V(j,4) = 0.0;
  V(j,5) = 0.0;
  V(j,6) = 1.0;
}

void SixDofEKF::makeBaseR() {
  float sc2x = 0.0015 ;
  float sc2 = 0.0015;
  float sc1 = 0.001; // * 0.001; //0.1 * 0.1;
  float sc3 = 0.001;

  R(1,1) = sc1;
  R(1,2) = 0.0;
  R(1,3) = 0.0;
  R(1,4) = 0.0;
  R(1,5) = 0.0;
  R(1,6) = 0.0;

  R(2,1) = 0.0;
  R(2,2) = sc3;
  R(2,3) = 0.0;
  R(2,4) = 0.0;
  R(2,5) = 0.0;
  R(2,6) = 0.0;

  R(3,1) = 0.0;
  R(3,2) = 0.0;
  R(3,3) = sc1;
  R(3,4) = 0.0;
  R(3,5) = 0.0;
  R(3,6) = 0.0;

  R(4,1) = 0.0;
  R(4,2) = 0.0;
  R(4,3) = 0.0;
  R(4,4) = sc2x;
  R(4,5) = 0.0;
  R(4,6) = 0.0;

  R(5,1) = 0.0;
  R(5,2) = 0.0;
  R(5,3) = 0.0;
  R(5,4) = 0.0;
  R(5,5) = sc2;
  R(5,6) = 0.0;

  R(6,1) = 0.0;
  R(6,2) = 0.0;
  R(6,3) = 0.0;
  R(6,4) = 0.0;
  R(6,5) = 0.0;
  R(6,6) = sc2;
}


void SixDofEKF::makeProcess() {
  Vector x_(x.size());

//  float roll = _adjustRoll(x(12));
//  float pitch = x(10);
//  float rroll = x(12);
//  float head  = x(11);

  int trigger = 0; // 15

  float g = x(12); // Roll
  float b = x(10); // Pitch
  float a = x(11); // Heading

  float ax = x(13);
  float ay = x(14);
  float az = x(15);

  // Break down the resulting vectors to components for easier debugging.
  float xa[3];
  float ya[3];
  float za[3];

  // Attempt three from  http://prt.fernuni-hagen.de/lehre/KURSE/PRT001/course_main_sh3d/node11.html
  xa[1] =  ax * cos(a) * cos(b);
  xa[2] =  ay * ( cos(a) * sin(b) * sin(g) - sin(a) * cos(g) );
  xa[3] =  az * ( cos(a) * sin(b) * cos(g) + sin(a) * sin(g) );

  x_(1) = xa[1] + xa[2] + xa[3];

  //#define DBUG_PROCSS 1
  #ifdef DBUG_PROCSS
  cout << " Components of X(" << x_(1) << ") = (" << xa[1] << ", " <<  xa[2] << ", " << xa[3]  << ")" << endl;
  #endif
  
  x_(2) = ins3DCalcs::velocity(x(2), x(1), period);
  x_(3) = ins3DCalcs::distance(x(3), x(2), x(1), period);
  
  ya[1] = ax * sin(a) * cos(b);
  ya[2] = ay * ( sin(a) * sin(b) * sin(g) + cos(a) * cos(g) );
  ya[3] = az * ( sin(a) * sin(b) * cos(g) - cos(a) * sin(g) );
  
  x_(4) = ya[1] + ya[2] + ya[3];

  #ifdef DBUG_PROCSS
  cout << " Components of Y(" << x_(4) << ") = (" << ya[1]  << ", " <<  ya[2]  << ", " <<  ya[3] << ")" << endl;
  #endif

  x_(5) = ins3DCalcs::velocity(x(5), x(4), period);
  x_(6) = ins3DCalcs::distance(x(6), x(5), x(4), period);

  za[1] = - ax * sin(b);
  za[2] = ay * cos(b) * sin(g);
  za[3] = az * cos(b) * cos(g);

  x_(7) = za[1] + za[2] + za[3] - 9.8; // - 9.8 - 0.025 / 2;
  #ifdef DBUG_PROCSS
  cout << " Components of Z(" << za[1] + za[2] + za[3] << ") = (" << za[1] << ", " <<  za[2] << ", " << za[3] << ")" << endl;
 #endif

  x_(8) = ins3DCalcs::velocity(x(9), x(8), period);
  x_(9) = ins3DCalcs::distance(x(10), x(9), x(8), period);

  count++;

//  float abs_pa = sqrt(x(13) * x(13) + x(14) * x(14) + x(15) * x(15));
//  float heading = acos( x(14) / abs_pa);
  float heading = atan2(x(13), x(14)) +  M_PI / 2;
//  float pitch = acos( x(13) / abs_pa) - M_PI / 2;
//  float roll = acos( x(15) / abs_pa);
/*float abs_pa = sqrt(x(1) * x(1) + x(4) * x(4) + x(7) * x(7));
float heading = atan2(x(1), x(4)) +  M_PI / 2;
float pitch = acos( x(1) / abs_pa) - M_PI / 2;
float roll = acos( x(7) / abs_pa);
*/
/*  if (heading > M_PI ) {
    heading = heading - M_PI * 2;
  }
*/
//  cout << "ESTHD " << heading * 57.29578 << "  " << x(11) * 57.29578<< endl;
  float abs_pa = sqrt(x(13) * x(13) + x(14) * x(14) + x(15) * x(15));
  float abs_pa2 = sqrt(x(14) * x(14) + x(15) * x(15));
  float pitch = acos( x(13) / abs_pa) - M_PI / 2;
  float roll =  - acos( x(14) / abs_pa2) + M_PI / 2;

#ifdef DBUG_PROCSS
  cout << " Heading (" << x(11) * 57.29578 << ", ^" << heading * 57.29578 << ")   pitch(" << x(10) * 57.29578 << ", " << pitch * 57.29578 << ")  roll(" << x(12) * 57.29578 << ", " << roll * 57.29578 << ")" << endl;
  #endif

  x_(10) = /* x(10); //*/ pitch;
  x_(11) = x(11);
  x_(12) = /* x(12); //*/ roll;
  x_(13) = x(13);
  x_(14) = x(14);
  x_(15) = x(15);

  x.swap(x_); 
}

void SixDofEKF::makeMeasure() {

  float abs_pa = sqrt(x(13) * x(13) + x(14) * x(14) + x(15) * x(15));
  float abs_pa2 = sqrt(x(14) * x(14) + x(15) * x(15));
//  float heading = acos( x(13) / abs_pa2) - M_PI / 2 ;
//  float heading = - acos( x(14) / abs_pa) + M_PI / 2 ;
  float heading = - atan2(x(14), x(13)) ;
  float pitch = acos( x(13) / abs_pa) - M_PI / 2;
  float roll =  - acos( x(14) / abs_pa2) + M_PI / 2;

  if (heading > 360) { heading = 0; }
  cout << count << " " << x(10) * 57.29578 << " " << x(11) * 57.29578  << " " << x(12) * 57.29578 << " " <<  pitch * 57.29578 << " " << heading * 57.29578 << " " << roll * 57.29578 << endl;

  z(1) =  pitch; //*/ x(10);
  z(2) = /* heading; */ x(11);
  z(3) =   roll; //*/ x(12);
  z(4) = x(13);
  z(5) = x(14);
  z(6) = x(15);
}

