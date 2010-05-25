#include "subHullLevelIndicator.h"
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoTransform.h>

#include "subHullOInv.h"
#include "subHull2d.h"

void SubHullLevelIndicator::updateSubmarinePitchLevelIndicator(float pitch) {
//  SoRotationXYZ *rotatorLli = (SoRotationXYZ *)(root->getChild(lliLineIndex));
//  printf("lli angle %f\n", rotatorLli->angle.getValue());
  pitchRotator->angle.setValue(-(pitch / 360) * 2 * M_PI);

  SbString *x = pitchText->string.startEditing();
  if (pitch < 0) {
    x[0].sprintf("pitch:%3.1f", pitch);
  } else {
    x[0].sprintf("pitch:\t%3.1f", pitch);
  }
  pitchText->string.finishEditing();

}

// Should only be called once.
// TODO(irox): Add check to prevent multiple creations.
SoSeparator *SubHullLevelIndicator::createSubmarinePitchLevelIndicator(float x, float y, float z) {

  SoSeparator *lliSep = new SoSeparator;
  lliSep->ref();

  SoCoordinate3 * lliCoords = new SoCoordinate3; //indicator
  SoCoordinate3 * llfCoords = new SoCoordinate3; // fixed line
//  SoSeparator *lliSep = root; // new SoSeparator;
  SoTransform *lliTrans = new SoTransform;
  SoResetTransform *lliReset = new SoResetTransform;
  lliSep->addChild(lliReset);
  lliTrans->translation.setValue(x, y, z);
  lliTrans->scaleFactor.setValue(0.5, 0.5, 0.5);
  lliSep->addChild(lliTrans);
    float llarr[2][3] = {{-12,0,0}, {12,0,0}};

  llfCoords->point.setValues(0,2,llarr);
  lliSep->addChild(llfCoords);
  SoLineSet *fixLine = new SoLineSet;
  fixLine->numVertices.setValue(2);
  lliSep->addChild(fixLine);

  // Pitch readout
  SbString readings[3] = {"Pitch: 000.0", "", ""};
  pitchText = new SoText2;
  pitchText->string.setValues(0, 3, readings);
  lliSep->addChild(pitchText);

/*  SoRotationXYZ* rot = new SoRotationXYZ;

  rot->axis.setValue(SoRotationXYZ::Y);
  rot->angle.setValue(1.50f);
  lliSep->addChild(rot);
*/
  // Add the rotator to perform pitch change rotations.
  pitchRotator = new SoRotationXYZ;
  pitchRotator->axis.setValue(SoRotationXYZ::Z);
  pitchRotator->angle.setValue(1.0f);
  lliSep->addChild(pitchRotator);

  SoRotationXYZ* rot = new SoRotationXYZ;

  rot->axis.setValue(SoRotationXYZ::Y);
  rot->angle.setValue(1.50f);
  lliSep->addChild(rot);

  // Draw lli line
  SoSeparator *rotLliSep = new SoSeparator;
  lliCoords->point.setValues(0,2,llarr);
  int verts[2] = {2,2};
  rotLliSep->addChild(lliCoords);
  SoLineSet *lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,1,verts);
  rotLliSep->addChild(lineSet);

  // Load the submarine hull mesh;.
//  rotLliSep->addChild(rotLliSep);
/*  SoRotationXYZ* rot = new SoRotationXYZ;

  rot->axis.setValue(SoRotationXYZ::Z);
  rot->angle.setValue(1.50f);
  rotLliSep->addChild(rot);
*/
  lliSep->addChild(createSubmarine());
// lliSep->addChild(create2dSubHull(0,0,0));
//  root->addChild(lliSep);
  return lliSep;
}

