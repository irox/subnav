// Copyright Ian Roxborough 2010

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoLineSet.h>

#include <sstream>

extern "C" {
void drawSolidScaledSubmarineHull(double scale, float slices);
void generateMesh(float mesh[][3], int);
void os5500GetABC(float hpr[3]);
void os5500Init(char *name);
}

int hullChildNum;
int hullRotatorXIndex;
int hullRotatorYIndex;
int hullRotatorZIndex;
int compassTextIndex;
int lliLineIndex;
int lliSepIndex;

char compassTextValue[100];
  SoText2 *compassText; //  = new SoText2;

float baseHpr[3];

void updateModel(void *data, SoSensor *) {
  SoSeparator *root = (SoSeparator*)data;

  SoRotationXYZ *rotatorX = (SoRotationXYZ *)(root->getChild(hullRotatorXIndex));
  SoRotationXYZ *rotatorY = (SoRotationXYZ *)(root->getChild(hullRotatorYIndex));
  SoRotationXYZ *rotatorZ = (SoRotationXYZ *)(root->getChild(hullRotatorZIndex));

  // Get compass data.
  float hpr[3];
  os5500GetABC(hpr);

  rotatorX->angle.setValue((hpr[1] / 360.0) * 2 * M_PI);
  rotatorY->angle.setValue(((baseHpr[0] - hpr[0]) / 360.0) * 2 * M_PI);
  rotatorZ->angle.setValue(-(hpr[2] / 360) * 2 * M_PI);

  // update lli.
  SoRotationXYZ *rotatorLli = (SoRotationXYZ *)(root->getChild(lliLineIndex));
  printf("lli angle %f\n", rotatorLli->angle.getValue());  
rotatorLli->angle.setValue(-(hpr[2] / 360) * 2 * M_PI);

  // Update HUD.
//  SoText2 *compassText = (SoText2 *)(root->getChild(compassTextIndex));
//  SbString x;
//  x.sprintf("%2.2f", hpr[0]);
//  SbString *v = new SbString;
//  sprintf(compassTextValue, "%2.2f", hpr[0]);
  SbString *x = compassText->string.startEditing();
  
  // Damn it, a hack to add the leading 00s on.
  if (hpr[0] < 10) {
    x[0].sprintf("00%3.1f", hpr[0]);
    x[1].sprintf("pitch:\t%3.1f", hpr[1]);
    x[2].sprintf("roll:\t%3.1f", hpr[2]);

  } else if (hpr[0] < 100) {
    x[0].sprintf("0%3.1f", hpr[0]);
  } else {
    x[0].sprintf("%3.1f", hpr[0]);
  }

  if (hpr[1] < 0) {
    x[1].sprintf("pitch:%3.1f", hpr[1]);
  } else {
    x[1].sprintf("pitch:\t%3.1f", hpr[1]);
  }
  if (hpr[2] < 0) {
    x[2].sprintf("roll:\t%3.1f", hpr[2]);
  } else {
    x[2].sprintf("roll:\t\t%3.1f", hpr[2]);
  }

  compassText->string.finishEditing();
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

  SoInput in;
  if (in.openFile("models/subhull-0.1.wrl")) {
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
    int slices = 8;
    int meshSize = (slices * 2 + 1) * 41;
    float subHullMesh[meshSize][3];
    generateMesh(subHullMesh, slices);
    coords->point.setValues(0, meshSize, subHullMesh);
    root->addChild(coords);
    SoQuadMesh * mesh = new SoQuadMesh;
    mesh->verticesPerRow = slices * 2 + 1;
    mesh->verticesPerColumn = 41;
    root->addChild(mesh);
  }

  // Transform for the conningtower.
  transform = new SoTransform;
  transform->translation.setValue(0.0, 3.0, 6.0);
  transform->scaleFactor.setValue(0.50, 1.0, 1.0);
  root->addChild(transform);

  // Cylinder for the conningtower.
  SoCylinder *cylinder = new SoCylinder;
  cylinder->radius.setValue(1.5);
  root->addChild(cylinder);

  // Tail Fins.
 
  // Make it and X tail.
  SoRotationXYZ *tailRot = new SoRotationXYZ;
//  tailRot->axis.setValue(SoRotationXYZ::Z);
//  tailRot->angle.setValue(M_PI/4.0f);
//  root->addChild(tailRot);

  transform = new SoTransform;
  transform->translation.setValue(0.0, -3.0, 12.0);
  transform->scaleFactor.setValue(0.10, 2.0, 1.0);
  root->addChild(transform);

  cylinder = new SoCylinder;
  cylinder->radius.setValue(0.5);
  root->addChild(cylinder);

  tailRot = new SoRotationXYZ;
  tailRot->axis.setValue(SoRotationXYZ::Z);
  tailRot->angle.setValue(M_PI/2.0f);
  root->addChild(tailRot);

  transform = new SoTransform;
  //transform->translation.setValue(0.0, -3.0, 12.0);
  transform->scaleFactor.setValue(0.0025, 40.0, 0.1250);
  root->addChild(transform);

  cylinder = new SoCylinder;
  cylinder->radius.setValue(3.5);
  root->addChild(cylinder);

return root;
}

int main(int argc, char **argv)
{
  // parse args
  // nsform = new SoTransform;
  //   transform->translation.setValue(0.0, -3.0, 12.0);
  //     transform->scaleFactor.setValue(0.10, 2.0, 1.0);
  //       root->addChild(transform);
  //       TODO:
  //
 
  os5500Init(NULL);
  os5500GetABC(baseHpr);
  Widget window = SoXt::init(argv[0]);

  if (window == NULL) exit(1);

  // Create scene root.
  SoSeparator *root = new SoSeparator;
  root->ref();

  // light
  root->addChild(new SoDirectionalLight);
  
// Set some material/color.
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue(1.0, 0.0, 0.0);
  root->addChild(material);

  SoRotationXYZ *hullRotator = new SoRotationXYZ;
  hullRotator->axis.setValue(SoRotationXYZ::Y);
  hullRotator->angle.setValue(1.5707963f);
  root->addChild(hullRotator);
  hullRotatorYIndex = root->findChild(hullRotator);

  hullRotator = new SoRotationXYZ;
  hullRotator->axis.setValue(SoRotationXYZ::X);
  hullRotator->angle.setValue(0.0f);
  root->addChild(hullRotator);
  hullRotatorXIndex = root->findChild(hullRotator);

  hullRotator = new SoRotationXYZ;
  hullRotator->axis.setValue(SoRotationXYZ::Z);
  hullRotator->angle.setValue(0.0f);
  root->addChild(hullRotator);
  hullRotatorZIndex = root->findChild(hullRotator);



  // Load the submarine hull mesh.
  root->addChild(createSubmarine());


  // HUD
  SoSeparator *textSep = new SoSeparator;
  SoTransform *textTrans = new SoTransform;
//  SoText2 *
  compassText = new SoText2;
  SoResetTransform *reset = new SoResetTransform;

  textTrans->translation.setValue(10.0, 10.0, 0.0);
  sprintf(compassTextValue, "0.0");
  SbString x(compassTextValue); // = new SbString(100);
//  x.sprintf("Heeelllllooo");
  SbString y("0.0");
  SbString z("0.0");
  SbString s[3] = { "Eriksen", "Blekken", "Aas" };

  compassText->string.setValues(0, 3, s );
 // "0.0";
//  compassText->string.set1Value(1, "001.01");
  textSep->renderCaching = SoSeparator::OFF;
  textSep->addChild(reset);
  textSep->addChild(textTrans);
  textSep->addChild(compassText);
  compassTextIndex = textSep->findChild(compassText);
  root->addChild(textSep);   

  // Level line indicator.
  SoCoordinate3 * lliCoords = new SoCoordinate3; //indicator
  SoCoordinate3 * llfCoords = new SoCoordinate3; // fixed line
  SoSeparator *lliSep = root; // new SoSeparator;
  SoTransform *lliTrans = new SoTransform;
  SoResetTransform *lliReset = new SoResetTransform;
  lliSep->addChild(lliReset);
  lliTrans->translation.setValue(15.0, 0.0, 0.0);
  lliSep->addChild(lliTrans);
    float llarr[2][3] = {{-4,0,0}, {4,0,0}};

  llfCoords->point.setValues(0,2,llarr);
  lliSep->addChild(llfCoords);
  SoLineSet *fixLine = new SoLineSet;
  fixLine->numVertices.setValue(2);
  lliSep->addChild(fixLine);

  hullRotator = new SoRotationXYZ;
  hullRotator->axis.setValue(SoRotationXYZ::Z);
  hullRotator->angle.setValue(1.0f);
  lliSep->addChild(hullRotator);
  lliLineIndex = lliSep->findChild(hullRotator);

  // Draw lli line
  SoSeparator *rotLliSep = new SoSeparator;
  lliCoords->point.setValues(0,2,llarr);
  int verts[2] = {2,2};
  rotLliSep->addChild(lliCoords);
  SoLineSet *lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,1,verts);
  rotLliSep->addChild(lineSet);
  // Load the submarine hull mesh.
  rotLliSep->addChild(rotLliSep);
  lliSep->addChild(createSubmarine());

//  root->addChild(lliSep);

  // The mechinism for updating the position.
  SoTimerSensor *sensor = new SoTimerSensor(updateModel, root);
  sensor->setInterval(0.04f); // 50 frames per second
  sensor->schedule();

  // Viewer widget.
  SoXtExaminerViewer *viewer = new SoXtExaminerViewer(window);

  // Rendering window setup.
  viewer->setSceneGraph(root);
  viewer->show();

  SoXt::show(window);
  SoXt::mainLoop();

  // Clean up.
  delete viewer;
  root->unref();

  return 0;
}

