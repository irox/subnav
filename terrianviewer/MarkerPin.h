#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText2.h>

class MarkerPin {

  public:
    MarkerPin();
    MarkerPin(char *, float, float, float, float);
    void setLocation(float lat, float lng);
    void setReferencePosition(float lat, float lng);
    void setLabel(char *);
    float getLattitude();
    float getLongitude();
    SoSeparator *getSoMarker();

  private:
    void updateMarkerText();
    void updateMarker();
    void initialize();

    SoSeparator *markerSeparator;
    SoTransform *transform;
    SoText2 *markerText;
    float referenceLat;
    float referenceLong;
    float currentLat;
    float currentLong;
    char *markerLabel;
};

