// Copyright Ian Roxborough 2010.
#include <cmath>
#include <iostream>

/*
 * 
 */

namespace ins3DCalcs {

 float velocity(float v0, float a, float t) {
   return v0 + a * t;
 }

 float distance(float d0, float v0, float a, float t) {
   return d0 + v0 * t + 0.5 * a * t * t;
 }
}
