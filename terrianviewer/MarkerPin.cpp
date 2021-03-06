#include <MarkerPin.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSphere.h>
#include <libnav/Position.hpp>
using namespace libnav;

MarkerPin::MarkerPin() {
  vesselTracks = true;
  initialize();
}

MarkerPin::MarkerPin(const char *name, float refLat, float refLong, float lat, float lng) {
  referenceLat = refLat;
  referenceLong = refLong;
  currentLat = lat;
  currentLong = lng;
  markerLabel = name;
  scaleFactor = 100000;
  altitude = 0.0;

  initialize();
  updateMarker();
}

void MarkerPin::setLocation(float lat, float lng, float alt) {
  currentLat = lat;
  currentLong = lng;
  altitude = alt;
  updateMarker();
}

void MarkerPin::setScalingFactor(int scale) {
  scaleFactor = scale;
}

void MarkerPin::updateMarker() {
  Position loc;
  loc.set_LLA(currentLat - referenceLat, currentLong - referenceLong, altitude, WGS84);

  float y = loc.get_y() / scaleFactor;
  float z = loc.get_z() / scaleFactor;

  transform->translation.setValue(-z, y, -0.4);

  if (vesselTracks) {
     int trackCount = vesselTrackCoords->point.getNum();
     vesselTrackCoords->point.insertSpace(trackCount, 1);
     SbVec3f * tracks = vesselTrackCoords->point.startEditing();

     Position zeroPos;
     zeroPos.set_LLA(0.0, 0.0, 0.0, WGS84);

     float x = (zeroPos.get_x() - loc.get_x()) / scaleFactor;
     if (trackCount == 1) {
       // Handle first position in the track.
       tracks[0].setValue(-z, y, -x);
     }

     tracks[trackCount].setValue(-z, y, -x);
     vesselTrackCoords->point.finishEditing();
  }

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

  vesselTrack = new SoLineSet();
  vesselTrackCoords = new SoCoordinate3();
  markerSeparator->addChild(vesselTrackCoords);
  markerSeparator->addChild(vesselTrack);

  markerSeparator->addChild(transform);
  markerSeparator->addChild(marker);
  markerSeparator->addChild(pinHeadTrans);
  markerSeparator->addChild(sphere);
  markerSeparator->addChild(pinTextTrans);
  markerSeparator->addChild(markerText);
}

void MarkerPin::setVesselTracks(bool value) {
  vesselTracks = value;
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

