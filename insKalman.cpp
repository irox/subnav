#include "insKalman.h"
#include <cmath>
#include <iostream>

using namespace std;

SingleAxisAccelEKF::SingleAxisAccelEKF() {
  setDim(3, 1, 1, 1, 1);
  Matrix qqQ(1,1);
  Q=qqQ;
  period = 1.0 / 40.0;
}

void SingleAxisAccelEKF::makeBaseA() {
  A(1, 1) = 1.0;
//  A(1, 2) = period;
//  A(1, 3) = 0.5 * period * period;
  A(2, 1) = 0.0;
  A(2, 2) = 1.0;
//  A(2, 3) = period;
  A(3, 1) = 0.0;
  A(3, 2) = 0.0;
  A(3, 3) = 1.0;
}

void SingleAxisAccelEKF::makeA() {
  A(1, 2) = period;
  A(1, 3) = 0.5 * period * period;
  A(2, 3) = period;
}

void SingleAxisAccelEKF::makeBaseW() {
  W(1,1) = 1.0;
  W(1,2) = 0.0;
  W(1,3) = 0.0;
}

void SingleAxisAccelEKF::makeBaseQ() {
  Q(1,1) = 0.01 * 0.01;
}

void SingleAxisAccelEKF::makeQ() {
  Q(1,1) = 0.01 * 0.01;
}

void SingleAxisAccelEKF::makeBaseH() {
  H(1,1) = 1.0;
  H(1,2) = period;
  H(1,3) = 0.5 * period * period;
}

void SingleAxisAccelEKF::makeH() {
  H(1,2) = period;
  H(1,3) = 0.5 * period * period;
}

void SingleAxisAccelEKF::makeBaseV() {
  V(1,1) = 1.0;
}

void SingleAxisAccelEKF::makeBaseR() {
  R(1,1) = 0.01*0.01;
}

void SingleAxisAccelEKF::makeProcess() {
  Vector x_(x.size());

  x_(1) = x(1);
  x_(2) = x(2) + x(1) * period;
  x_(3) = x(3) + x(2) * period + 0.5 * x(1) * period * period;

  x.swap(x_); 
}

void SingleAxisAccelEKF::makeMeasure() {
  z(1) = x(1);
}

