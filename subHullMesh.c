
#include <math.h>

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


void generateMesh(float mesh[][3], int slices) {

  float angle, rad, distance;
  int vert = 0;
  int last = 20;
  int i;
  // Draw the end of sub around the main view port.
/*  rad = framepointsXY[0][1];
  distance = framepointsXY[0][0];
  for( angle = 0.0f; angle < (2.0 * M_PI); angle += (M_PI/slices)) {
    mesh[vert][0] = (rad - rad * 0.175) * sin(angle);
    mesh[vert][1] = (rad - rad * 0.175) * cos(angle);
    mesh[vert][2] = distance;
    vert++;
  }
  */                    
  // Draw the outside of the hull.
  for (i=0; i < last; i++) {
    distance = framepointsXY[i][0];
    rad = framepointsXY[i][1];
    for( angle = 0.0f; angle < (2.0 * M_PI); angle += (M_PI/slices)) {
      mesh[vert][0] = rad * sin(angle);
      mesh[vert][1] = rad * cos(angle);
      mesh[vert][2] = distance;
      vert++;
    }
    // Last vertice in the row to connect the ends of the row.
    mesh[vert][0] = rad * sin(0.0f);
    mesh[vert][1] = rad * cos(0.0f);
    mesh[vert][2] = distance;
    vert++;
  }

  // Draw the inside of the hull.
  for (i=last; i >= 0; i--) {
    distance = framepointsXY[i][0];
    rad = framepointsXY[i][1];
    for( angle = 0.0f; angle < (2.0 * M_PI); angle += (M_PI/slices)) {
      mesh[vert][0] = (rad - rad * 0.175) * sin(angle);
      mesh[vert][1] = (rad - rad * 0.175) * cos(angle);
      mesh[vert][2] = distance;
      vert++;
    }
    // Last vertice in the row to connect the ends of the row.
    mesh[vert][0] = (rad - rad * 0.175) * sin(0.0f);
    mesh[vert][1] = (rad - rad * 0.175) * cos(0.0f);
    mesh[vert][2] = distance;
    vert++;
  }

  // Draw the end of sub around the main view port.
/*  rad = framepointsXY[0][1];
  distance = framepointsXY[0][0];
  for( angle = 0.0f; angle < (2.0 * M_PI); angle += (M_PI/slices)) {
    mesh[vert][0] = rad * sin(angle);
    mesh[vert][1] = rad * cos(angle);
    mesh[vert][2] = distance;
    vert++;
  }*/
  printf("total vertices: %d\n", vert);
}
