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
};
