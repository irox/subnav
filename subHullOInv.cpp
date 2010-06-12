#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoResetTransform.h>



extern "C" {
//void drawSolidScaledSubmarineHull(double scale, float slices);
void generateHullMesh(float mesh[][3], int);
void generateForwardMBT(float mesh[][3], int);
}

SoSeparator* createSubmarine() {
  SoSeparator *root = new SoSeparator;
  root->ref();


  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue(1.0, 0.2, 0.2);
  material->shininess.setValue(1.0);
  root->addChild(material);

  // Center the hull.
  SoTransform *transform = new SoTransform;
  transform->translation.setValue(0.0, 0.0, -9.0);
  root->addChild(transform);

  int slices = 8;
  SoInput in;
  if ( in.openFile("models/subhull-0.1.wrl")) {
    SoRotationXYZ *hullRot =  new SoRotationXYZ;
    hullRot->axis.setValue(SoRotationXYZ::Y);
    hullRot->angle.setValue(-M_PI/2.0f);
    root->addChild(hullRot);

    SoSeparator *hull = SoDB::readAll(&in);
    root->addChild(hull);

    SoRotationXYZ *hullRotBack =  new SoRotationXYZ;
    hullRotBack->axis.setValue(SoRotationXYZ::Y);
    hullRotBack->angle.setValue(M_PI/2.0f);
    root->addChild(hullRotBack);


  } else {

    // Load the submarine hull mesh.
    SoCoordinate3 * coords = new SoCoordinate3;
    int meshSize = (slices * 2 + 1) * 41;
    float subHullMesh[meshSize][3];
    generateHullMesh(subHullMesh, slices);
    coords->point.setValues(0, meshSize, subHullMesh);
    root->addChild(coords);
    SoQuadMesh * mesh = new SoQuadMesh;
    mesh->verticesPerRow = slices * 2 + 1;
    mesh->verticesPerColumn = 41;
    root->addChild(mesh);
  }


  SoSeparator *otherStuff = new SoSeparator();
  // generateForwardMBT
  int meshSize = (slices * 2 + 1) * 11;
  float subFMBTMesh[meshSize][3];
  generateForwardMBT(subFMBTMesh, slices);
  SoCoordinate3 * mbtCoords = new SoCoordinate3;
  mbtCoords->point.setValues(0, meshSize, subFMBTMesh);
  otherStuff->addChild(mbtCoords);
  SoQuadMesh * mbtMesh = new SoQuadMesh;
  mbtMesh->verticesPerRow = slices * 2 + 1;
  mbtMesh->verticesPerColumn = 11;
  otherStuff->addChild(mbtMesh);

  // Transform for the conningtower.
    transform = new SoTransform;
  transform->translation.setValue(0.0, 3.0, 6.0);
  transform->scaleFactor.setValue(0.50, 1.0, 1.0);
  otherStuff->addChild(transform);

  // Cylinder for the conningtower.
    SoCylinder *cylinder = new SoCylinder;
  cylinder->radius.setValue(1.5);
  otherStuff->addChild(cylinder);

  // Tail Fins.
  //
  //   // Make it and X tail.

  SoRotationXYZ *tailRot = new SoRotationXYZ;
  transform = new SoTransform;
  transform->translation.setValue(0.0, -3.0, 12.0);
  transform->scaleFactor.setValue(0.10, 2.0, 1.0);
  otherStuff->addChild(transform);

  cylinder = new SoCylinder;
  cylinder->radius.setValue(0.5);
//  otherStuff->addChild(cylinder);
  transform = new SoTransform;
  transform->scaleFactor.setValue(10, 0.5, 1.0); //0.0025, 40.0, 0.1250);
  otherStuff->addChild(transform);

  tailRot = new SoRotationXYZ;
  tailRot->axis.setValue(SoRotationXYZ::Z);
  tailRot->angle.setValue(M_PI/2.0f);
  otherStuff->addChild(tailRot);

//  cylinder = new SoCylinder;
//  cylinder->radius.setValue(3.5);
  root->addChild(otherStuff);

  // V-Tail fins
  SoInput inTailFin;
  if ( inTailFin.openFile("models/tailfinV.wrl")) {
    SoResetTransform *transReset = new SoResetTransform;
//    root->addChild(transReset);

    SoSeparator *tailSep = new SoSeparator();
    

    SoRotationXYZ *vtailRot = new SoRotationXYZ;
//    vtailRot->axis.setValue(SoRotationXYZ::Z);
//    vtailRot->angle.setValue(-M_PI/2.0f);
//    tailSep->addChild(vtailRot);
//    vtailRot = new SoRotationXYZ;
    vtailRot->axis.setValue(SoRotationXYZ::Y);
    vtailRot->angle.setValue(-M_PI/2.0f);
    tailSep->addChild(vtailRot);

    transform = new SoTransform;
 //   transform->scaleFactor.setValue(1, 1, 1);
    transform->translation.setValue(17.00, 0, 0);
    tailSep->addChild(transform);



    vtailRot = new SoRotationXYZ;
    vtailRot->axis.setValue(SoRotationXYZ::Z);
    vtailRot->angle.setValue(-M_PI/3.0f);
//    root->addChild(vtailRot);

//    transform = new SoTransform;
 //   transform->scaleFactor.setValue(1, 1, 1);
 //   transform->translation.setValue(16.00, 2, 2);
//    tailSep->addChild(transform);


    SoSeparator *vtail1 = SoDB::readAll(&inTailFin);
    tailSep->addChild(vtail1);

/********************************************************/
/*
    transform = new SoTransform;
 //   transform->scaleFactor.setValue(1, 1, 1);
    transform->translation.setValue(0.0 , -4.5, -4.0);
    tailSep->addChild(transform);
            
    vtailRot = new SoRotationXYZ;
    vtailRot->axis.setValue(SoRotationXYZ::X);
    vtailRot->angle.setValue(-4*M_PI/6.0f);
    tailSep->addChild(vtailRot);
 
    SoInput inTailFin2;
    inTailFin2.openFile("models/tailfin1.wrl");
    SoSeparator *vtail2 = SoDB::readAll(&inTailFin2);
   // tailSep->addChild(vtail2);

    // Reset the rotation...

    vtailRot = new SoRotationXYZ;
    vtailRot->axis.setValue(SoRotationXYZ::Z);
    vtailRot->angle.setValue(-M_PI/6.0f);
    tailSep->addChild(vtailRot);
*/

    root->addChild(tailSep);

  //  vtail1 = SoDB::readAll(&inTailFin);
 //   root->addChild(vtail1);

  }

  SoInput inDeck;
  if (inDeck.openFile("models/mbtdeck1.wrl")) {
    
    SoRotationXYZ *deckRot = new SoRotationXYZ;
    deckRot->axis.setValue(SoRotationXYZ::Z);
    deckRot->angle.setValue(-M_PI/2.0f);
//    root->addChild(deckRot);

    deckRot = new SoRotationXYZ;
    deckRot->axis.setValue(SoRotationXYZ::Y);
    deckRot->angle.setValue(M_PI/1.0f);
    root->addChild(deckRot);

    transform = new SoTransform;
    transform->scaleFactor.setValue(0.8, 1, 1);
    transform->translation.setValue(/*0.20*/0, 0.65, -2.350);
    root->addChild(transform);
 //
    SoSeparator *deck1 = SoDB::readAll(&inDeck);
    root->addChild(deck1);

  }
  return root;
}

