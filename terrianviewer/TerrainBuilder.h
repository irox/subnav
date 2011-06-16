#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>

class TerrainBuilder {

  public:
    void setHeight(int height);
    void setWidth(int width);
    void loadXYZFile(char *xyzFilename, int pointCount);
    SoCoordinate3 *getMapCoordinates();
    SoTexture2 *getTexture();
    SoTextureCoordinate2 *getTextureCoordinates();
    SoNormal *getNormals();
    void initialize();
    float getRefLong();
    float getRefLat();
    enum ColorMap {
      STANDARD_COLOR = 0,
      EXPERIMENTAL_COLOR = 1
    };

  private:
    // The map points.
    SoTexture2 * texture;
    SoTextureCoordinate2 * texture_coords;
    SoCoordinate3 * coords;
    SoNormal * normals;
    SoNormalBinding * normal_binding;
    unsigned char *imageMap;

    int terrainHeight;
    int terrainWidth; 
    float ref_lat;
    float ref_long;

    int getRed(float);
    int getBlue(float);
    int getGreen(float);
    void processColorFor(float);
    void standardProcessColorFor(float);
    void experimentalProcessColorFor(float);

    int colorMap;
    float lastDepth;
    bool forceColorMapCalc;
    int skipColorMapCount;
    int lastBlue;
    int lastRed;
    int lastGreen;
};
