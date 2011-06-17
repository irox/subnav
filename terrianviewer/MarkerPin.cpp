#include <MarkerPin.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSphere.h>
#include <libnav/Position.hpp>
using namespace libnav;

MarkerPin::MarkerPin() {
  initialize();
}

MarkerPin::MarkerPin(const char *name, float refLat, float refLong, float lat, float lng) {
  referenceLat = refLat;
  referenceLong = refLong;
  currentLat = lat;
  currentLong = lng;
  markerLabel = name;

  initialize();
  updateMarker();
}

void MarkerPin::setLocation(float lat, float lng) {
  currentLat = lat;
  currentLong = lng;
  updateMarker();
}

void MarkerPin::updateMarker() {
  Position loc;
  loc.set_LLA(currentLat - referenceLat, currentLong - referenceLong, 0.0, WGS84);

  float y = loc.get_y() / 100000;
  float z = loc.get_z() / 100000;

  transform->translation.setValue(-z, y, -0.4);
  updateMarkerText();
}

SoSFVec3f MarkerPin::getPos() {
  return transform->translation;
}

void MarkerPin::setReferencePosition(float lat, float lng) {
  referenceLat = lat;
  referenceLong = lng;
}

void MarkerPin::setLabel(const char *name) {
  markerLabel = name;
}

float MarkerPin::getLongitude() {
  return currentLong;
}

float MarkerPin::getLattitude() {
  return currentLat;
}

void MarkerPin::initialize() {
  markerSeparator = new SoSeparator();

  markerText = new SoText2();
  SbString markerTexts[2] = {"Marker_name", "x, y"};
  markerText->string.setValues(0, 2, markerTexts);

  SoMaterial *material = new SoMaterial;
  material->ambientColor.setValue(0.0, 0.0, 0.8);
  material->diffuseColor.setValue(0.8, 0.8, 0.8);
  material->transparency.setValue(0.0);
  material->shininess.setValue(0.6);
  material->emissiveColor.setValue(0.000000, 0.000000, 0.000000);

  transform = new SoTransform;

  markerSeparator->addChild(new SoTexture2());

  markerSeparator->addChild(material);
  SoCube *marker = new SoCube();
  marker->width  = 1.0  / 500;
  marker->height = 1.0f / 500;
  marker->depth  = 1.0f / 1;
  SoTransform *pinHeadTrans = new SoTransform();
  pinHeadTrans->translation.setValue(0,0,0.5);

  SoSphere *sphere = new SoSphere();
  sphere->radius = 0.02f;

  SoTransform *pinTextTrans = new SoTransform();
  pinTextTrans->translation.setValue(0,0,0.05);

  markerSeparator->addChild(transform);
  markerSeparator->addChild(marker);
  markerSeparator->addChild(pinHeadTrans);
  markerSeparator->addChild(sphere);
  markerSeparator->addChild(pinTextTrans);
  markerSeparator->addChild(markerText);
}

void MarkerPin::updateMarkerText() {
  SbString *str = markerText->string.startEditing();

  if (markerLabel) {
    str[0].sprintf("%s", markerLabel);
  }

  str[1].sprintf("%3.3f, %3.3f", currentLong, currentLat);
  markerText->string.finishEditing();
}

SoSeparator *MarkerPin::getSoMarker() {
  return markerSeparator;
}

