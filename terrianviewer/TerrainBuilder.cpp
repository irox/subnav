#include "TerrainBuilder.h"
#include <libnav/Position.hpp>
using namespace libnav;

void TerrainBuilder::setHeight(int height) {
  terrainHeight = height;
}


void TerrainBuilder::setWidth(int width) {
  terrainWidth = width;
}

int TerrainBuilder::getBlue(float depth) {
  processColorFor(depth);
  return lastBlue;
}

int TerrainBuilder::getRed(float depth) {
  processColorFor(depth);
  return lastRed;}

int TerrainBuilder::getGreen(float depth) {
  processColorFor(depth);
  return lastGreen;
}

void TerrainBuilder::processColorFor(float depth) {
  if (depth != lastDepth) {
    switch(colorMap) {
      case TerrainBuilder::STANDARD_COLOR:
        standardProcessColorFor(depth);
        break;
      case TerrainBuilder::EXPERIMENTAL_COLOR:
        experimentalProcessColorFor(depth);
      default:
        experimentalProcessColorFor(depth);
    }
    lastDepth = depth;
  }
}

void TerrainBuilder::standardProcessColorFor(float depth) {
  if (depth> -150 && depth <= 0) {
    if (depth >= -2.0) {
      depth = -2.0;
    }
    lastBlue = int(depth * 1.7);
    lastGreen = 0;
    lastRed = 0;
  } else if (depth < -300 && depth > -600) {
    lastBlue = 0;
    lastGreen = int(depth / 1.33);
    lastRed = 225;
  } else if (depth < -600) {
    lastBlue = depth < 0 ? depth / 23 : 0;
    lastGreen = 0;
    lastRed = depth < 0 ? depth / 23 : 0;
  } else {
    lastBlue = depth < 0 ? depth / 23 : 0;
    lastGreen = depth > 0 ? int(depth) % 255 : int(depth) %  250;
    lastRed = depth > 0 ? 50 + depth / 15 : 0;
  }
  lastDepth = depth;
}

bool isBoundary(float lastDepth, float depth, float targetDepth) {
  return ((depth <= targetDepth && lastDepth >= targetDepth) ||
          (depth >= targetDepth && lastDepth <= targetDepth));
}
/* Depth coloring in bands based on:
 * scuba depth
 * K250 depth
 * K350 depth
 * the rest
 */
void TerrainBuilder::experimentalProcessColorFor(float depth) {
  int contourSpacing = 1000;
  int contourZone = int(depth / contourSpacing);
  int oldContourZone = int(lastDepth / contourSpacing);
  if (depth < -10 && contourZone != oldContourZone) {
    /* draw contours */
    lastBlue = 255;
    lastRed = 255;
    lastGreen = 255;
  } else if (isBoundary(lastDepth, depth, -40)) {
    /* SCUBA max contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
  } else if (isBoundary(lastDepth, depth, -75)) {
    /* K250 max depth contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
  } else if (isBoundary(lastDepth, depth, -105)) {
    /* K350 max depth contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
  } else if (isBoundary(lastDepth, depth, -180)) {
    /* K600 max depth contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
  } else if (depth> -40 && depth <= 0) {
    /* SCUBA depth */
    if (depth >= -2.0) {
      depth = -2.0;
    }
    lastBlue = 255;
    lastRed = 0;
    lastGreen = 165 + int(depth*2);
  } else if (depth> -75 && depth <= -40) {
    /* K250 depth */
    lastBlue = 255 + (40 + int(depth)) * 3;
    lastRed = 0;
    lastGreen = 80 + (40 + int(depth));
  } else if (depth> -105 && depth <= -75) {
    /* K350 depth */
    lastBlue = 150 + (75 + int(depth)) * 2 ;
    lastRed = 0;
    lastGreen = 40 + (75 + int(depth));
  } else if (depth < -300 && depth > -600) {
    lastBlue = 0;
    lastGreen = int(depth / 1.33);
    lastRed = 225;
  } else if (depth < -600) {
    lastBlue = depth < 0 ? depth / 23 : 0;
    lastGreen = 0;
    lastRed = depth < 0 ? depth / 23 : 0;
  } else {
    lastBlue = depth < 0 ? depth / 23 : 0;
    lastGreen = depth > 0 ? int(depth) % 255 : int(depth) %  250;
    lastRed = depth > 0 ? 50 + depth / 15 : 0;
  }
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

  // Offset handling is currently broken.  (Fix or remove)
  int xoffset = 0;
  int yoffset = 0;
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

    if (( col_count >= xoffset && col_count < terrainWidth + xoffset ) &&
        ( row_count >= yoffset && row_count < terrainWidth + yoffset)) {
      loc.set_LLA(lat - first_lat, lng - first_lng, zvalue, WGS84);

      x = (first.get_x() - loc.get_x()) / 100000;
      y = loc.get_y() / 100000;
      z = loc.get_z() / 100000;

      pointTerrainCount = col_count + xoffset + (row_count + yoffset) * terrainWidth;
      
      // Calculate pixel colo(u)r...
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
        imageMap[imageIndex++] = getRed(zvalue);
        imageMap[imageIndex++] = getGreen(zvalue);
        imageMap[imageIndex++] = getBlue(zvalue);
      }
      points[pointTerrainCount] = SbVec3f(-z, y, -x);
      texture_points[pointCount] = SbVec2f(
          (col_count + xoffset) * 1.0 / (terrainWidth + xoffset) * 1.0,
          (row_count - yoffset) * 1.0 / (terrainHeight) * 1.0);
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

  colorMap = TerrainBuilder::EXPERIMENTAL_COLOR;
  lastDepth = -999999;
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

