#include <math.h>
#include "subHull2d.h"
 
float framepointsXYZ[20][3] = {
            { 0.65, 0.75, 0 },
            { 1, 0.977, 0 },
            { 2, 1.567, 0 },
            { 3, 1.93, 0 },
            { 4, 2.098, 0 },
            { 5, 2.215, 0 },
            { 6, 2.284, 0 },
            { 7, 2.307, 0 },
            { 8, 2.303, 0 },
            { 9, 2.292, 0 },
            { 10, 2.262, 0 },
            { 11, 2.186, 0},
            { 12, 2.086, 0},
            { 13, 1.894, 0 },
            { 14, 1.683, 0 },
            { 15, 1.414, 0 },
            { 16, 1.095, 0 },
            { 17, 0.767, 0 },
            { 18, 0.381, 0 },
            { 19, 0.04, 0}};

// This appears rather broken just now. TODO: fix!
SoSeparator* create2dSubHull(float x, float y, float z) {
  SoSeparator *root = new SoSeparator;

  for(int i=0; i < 20; i++) {
    SoCoordinate3 * coords = new SoCoordinate3;
    coords->point.setValues(0,20,framepointsXYZ);
    root->addChild(coords);

    SoLineSet *line = new SoLineSet;
    line->numVertices.setValue(20);
    root->addChild(line);
  }

  // invert frame points.
  for(int i=0; i < 20; i++) {
    framepointsXYZ[i][1]=-framepointsXYZ[i][1];
  }

  for(int i=0; i < 20; i++) {
    SoCoordinate3 * coords = new SoCoordinate3;
    coords->point.setValues(0,20,framepointsXYZ);
    root->addChild(coords);

    SoLineSet *line = new SoLineSet;
    line->numVertices.setValue(20);
    root->addChild(line);
  }
  
  return root;
}

