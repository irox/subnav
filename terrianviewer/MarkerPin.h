#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>

class MarkerPin {

  public:
    MarkerPin();
    MarkerPin(const char *, float, float, float, float);
    void setLocation(float lat, float lng);
    void setReferencePosition(float lat, float lng);
    void setLabel(const char *);
    void setVesselTracks(bool);
    float getLattitude();
    float getLongitude();
    SoSeparator *getSoMarker();
    SoSFVec3f getPos();

  private:
    void updateMarkerText();
    void updateMarker();
    void initialize();

    SoSeparator *markerSeparator;
    SoTransform *transform;
    SoText2 *markerText;
    SoCoordinate3 *vesselTrackCoords;
    SoLineSet *vesselTrack;

    float referenceLat;
    float referenceLong;
    float currentLat;
    float currentLong;
    const char *markerLabel;
    bool vesselTracks;
};

