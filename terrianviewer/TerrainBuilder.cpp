#include "TerrainBuilder.h"
#include <libnav/Position.hpp>
using namespace libnav;

void TerrainBuilder::setHeight(int height) {
  terrainHeight = height;
}


void TerrainBuilder::setWidth(int width) {
  terrainWidth = width;
}

void TerrainBuilder::loadXYZFile(char *filename, int dataCount) {
  int pointCount = 0;
  int pointTerrainCount = 0;
  float x = 0, y = 0;
  float z = 0;
  float zvalue = 0;

  float lat = 0;
  float lng = 0;

  float first_lat = 0;
  float first_lng = 0;

  float current_lat = 999;

  int I = 0;
  int imageIndex = 0;

  int row_count = -1;
  int col_count = 0;
  Position first, loc;

  unsigned char *imageMap = new unsigned char[terrainWidth * terrainHeight * 3];

  SbVec3f * points = coords->point.startEditing();
  SbVec2f * texture_points = texture_coords->point.startEditing();
  SbVec3f * normal_points = normals->vector.startEditing();

  FILE *hmFile = fopen (filename , "r");

  for (I = 0; I < dataCount; I++)
  {
    fscanf(hmFile, "%f %f %f", &lng, &lat, &zvalue);

    if (current_lat == 999) {
      // Set first position (everything is should drawn relative to this point.
      first.set_LLA(0.0, 0.0, 0.0, WGS84);
      ref_lat = first_lat = lat;
      ref_long = first_lng = lng;
    }

    if (current_lat != lat) {
      // We are starting a new row.
      current_lat = lat;
      col_count = terrainWidth - 1;
      row_count++;
    } else {
      col_count--;
    }

    if (( col_count >= 0 ) && ( row_count < terrainWidth)) {
      loc.set_LLA(lat - first_lat, lng - first_lng, zvalue, WGS84);

      x = (first.get_x() - loc.get_x()) / 100000;
      y = loc.get_y() / 100000;
      z = loc.get_z() / 100000;

      pointTerrainCount = col_count + row_count * terrainWidth;
      
      // Calculate pixel colo(u)r...
      int red, green, blue;

      if (zvalue > -150 && zvalue <= 0) {
        if (zvalue >= -2.0) {
          zvalue = -2.0;
        }
        blue = int(zvalue * 1.7);
        green = 0;
        red = 0;
      } else if (zvalue < -300 && zvalue > -600) {
        blue = 0;
        green = int(zvalue / 1.33);
        red = 225;
      } else if (zvalue < -600) {
        blue = zvalue < 0 ? zvalue / 23 : 0;
        green = 0;
        red = zvalue < 0 ? zvalue / 23 : 0;
      } else {
        blue = zvalue < 0 ? zvalue / 23 : 0;
        green = zvalue > 0 ? int(zvalue) % 255 : int(zvalue) %  250;
        red = zvalue > 0 ? 50 + zvalue / 15 : 0;
      }

      if (imageIndex % (terrainWidth * 3) < 1) {
        imageMap[imageIndex++] = 0;
        imageMap[imageIndex++] = 0;
        imageMap[imageIndex++] = 255;
 
      } else if (imageIndex < 6 * terrainWidth) {
        imageMap[imageIndex++] = 255;
        imageMap[imageIndex++] = 0;
        imageMap[imageIndex++] = 0;
      } else if (imageIndex % (terrainWidth * 3) < 6) {
        imageMap[imageIndex++] = 255;
        imageMap[imageIndex++] = 255;
        imageMap[imageIndex++] = 255;
      } else {
        imageMap[imageIndex++] = red;
        imageMap[imageIndex++] = green;
        imageMap[imageIndex++] = blue;
      }
      points[pointTerrainCount] = SbVec3f( 3.79087-z, y, -x );
      texture_points[pointCount] = SbVec2f(col_count * 1.0/ terrainWidth * 1.0,
                                           row_count * 1.0 / terrainHeight * 1.0);
      pointCount++;
    }
  }

  SbVec2s imageSize(terrainWidth, terrainHeight);
  texture->image.setValue(imageSize, 3, imageMap, SoSFImage::COPY);

  /* Compute inner normals. */
  for (int Y = 1; Y < (terrainHeight - 1); ++Y)
  {
    for (int X = 1; X < (terrainWidth - 1); ++X)
    {
      int index = Y * terrainWidth + X;
      SbVec3f normal = SbVec3f(0.0f, 0.0f, 0.0f);

      normal += (points[index - 1] - points[index]).cross(points[index
        - terrainWidth] - points[index]);
      normal += (points[index -terrainWidth] - points[index]).cross(points[index
        - terrainWidth + 1] - points[index]);
      normal += (points[index - terrainWidth + 1] - points[index]).cross(points[index
        + 1] - points[index]);
      normal += (points[index + 1] - points[index]).cross(points[index
        + terrainWidth] - points[index]);
      normal += (points[index + terrainWidth] - points[index]).cross(points[index
        + terrainWidth - 1] - points[index]);
      normal += (points[index + terrainWidth - 1] - points[index]).cross(points[index
        - 1] - points[index]);
      normal.normalize();
      normal_points[index] = normal;
    }
  }

  /* Compute normals at top and bottom border. */
  for (int X = 1; X < (terrainWidth - 1); ++X)
  {
    int index_1 = X;
    int index_2 = (terrainHeight - 1) * terrainWidth + X;
    SbVec3f normal_1 = SbVec3f(0.0f, 0.0f, 0.0f);
    SbVec3f normal_2 = SbVec3f(0.0f, 0.0f, 0.0f);

    /* Top border. */
    normal_1 += (points[index_1 + 1] - points[index_1]).cross(points[index_1
      + terrainWidth] - points[index_1]);
    normal_1 += (points[index_1 + terrainWidth] - points[index_1]).cross(points[index_1
      + terrainWidth - 1] - points[index_1]);
    normal_1 += (points[index_1 + terrainWidth - 1] - points[index_1]).cross(points[index_1
      - 1] - points[index_1]);

    /* Bottom border. */
    normal_2 += (points[index_2 - 1] - points[index_2]).cross(points[index_2
      - terrainWidth] - points[index_2]);
    normal_2 += (points[index_2 - terrainWidth] - points[index_2]).cross(points[index_2
      - terrainWidth + 1] - points[index_2]);
    normal_2 += (points[index_2 - terrainWidth + 1] - points[index_2]).cross(points[index_2
      + 1] - points[index_2]);

    normal_1.normalize();
    normal_2.normalize();
    normal_points[index_1] = normal_1;
    normal_points[index_2] = normal_2;
  }

  /* Compute normals at left and right border. */
  for (int Y2 = 1; Y2 < (terrainHeight - 1); ++Y2)
  {
    int index_1 = Y2 * terrainWidth;
    int index_2 = index_1 + terrainWidth - 1;
    SbVec3f normal_1 = SbVec3f(0.0f, 0.0f, 0.0f);
    SbVec3f normal_2 = SbVec3f(0.0f, 0.0f, 0.0f);

    /* Left border. */
    normal_1 += (points[index_1 - terrainWidth] - points[index_1]).cross(points[index_1
      - terrainWidth + 1] - points[index_1]);
    normal_1 += (points[index_1 - terrainWidth + 1] - points[index_1]).cross(points[index_1
      + 1] - points[index_1]);
    normal_1 += (points[index_1 + 1] - points[index_1]).cross(points[index_1
      + terrainWidth] - points[index_1]);

    /* Right border. */
    normal_2 += (points[index_2 - 1] - points[index_2]).cross(points[index_2
      - terrainWidth] - points[index_2]);
    normal_2 += (points[index_2 + terrainWidth] - points[index_2]).cross(points[index_2
      + terrainWidth - 1] - points[index_2]);
    normal_2 += (points[index_2 + terrainWidth - 1] - points[index_2]).cross(points[index_2
      - 1] - points[index_2]);

    normal_1.normalize();
    normal_2.normalize();
    normal_points[index_1] = normal_1;
    normal_points[index_2] = normal_2;
  }

  /* Compute normals in corners. */
  int index;
  SbVec3f normal;

  index = 0;
  normal = (points[index + 1] - points[index]).cross(points[index + terrainWidth]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  index = (terrainHeight * terrainWidth) - 1;
  normal = (points[index - 1] - points[index]).cross(points[index - terrainWidth]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  index = (terrainHeight - 1) * terrainWidth;
  normal = (points[index - terrainWidth] - points[index]).cross(points[index - terrainWidth + 1]
    - points[index]);
  normal += (points[index - terrainWidth + 1] - points[index]).cross(points[index + 1]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  index = terrainWidth - 1;
  normal += (points[index + terrainWidth] - points[index]).cross(points[index + terrainWidth - 1]
    - points[index]);
  normal += (points[index + terrainWidth - 1] - points[index]).cross(points[index - 1]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  coords->point.finishEditing();
  texture_coords->point.finishEditing();
  normals->vector.finishEditing();
}

void TerrainBuilder::initialize() {
  texture = new SoTexture2();
  texture_coords = new SoTextureCoordinate2();
  coords = new SoCoordinate3();
  normals = new SoNormal();
  normal_binding = new SoNormalBinding();

  coords->point.setNum(terrainWidth * terrainHeight);
  texture_coords->point.setNum(terrainWidth * terrainHeight );
  normals->vector.setNum(terrainWidth * terrainHeight);
  normal_binding->value.setValue(SoNormalBinding::PER_VERTEX_INDEXED);

}

SoCoordinate3 *TerrainBuilder::getMapCoordinates() {
  return coords;
}

SoTextureCoordinate2 *TerrainBuilder::getTextureCoordinates() {
  return texture_coords;
}

SoTexture2 *TerrainBuilder::getTexture() {
  return texture;
}

SoNormal *TerrainBuilder::getNormals() {
  return normals;
}

float TerrainBuilder::getRefLong() {
  return ref_long;
}

float TerrainBuilder::getRefLat() {
  return ref_lat;
}

