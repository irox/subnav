#include "TerrainBuilder.h"
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoQuadMesh.h>

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
  if ((depth != lastDepth || forceColorMapCalc) && skipColorMapCount < 1) {
    forceColorMapCalc = false;
    switch(colorMap) {
      case TerrainBuilder::STANDARD_COLOR:
        standardProcessColorFor(depth);
        break;
      case TerrainBuilder::EXPERIMENTAL_COLOR:
        experimentalProcessColorFor(depth);
      default:
        K350DepthColor(depth); //experimentalProcessColorFor(depth);
    }
    lastDepth = depth;
  } else {
   skipColorMapCount--;
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
  return ((depth <= targetDepth && lastDepth > targetDepth) ||
          (depth >= targetDepth && lastDepth < targetDepth));
}

void TerrainBuilder::K350DepthColor(float depth) {
  int contourSpacing = 500;
  int contourZone = int(depth / contourSpacing);
  int oldContourZone = int(lastDepth / contourSpacing);
  if (depth < -10 && contourZone != oldContourZone) {
    /* draw contours */
    lastBlue = 255;
    lastRed = 255;
    lastGreen = 255;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (isBoundary(lastDepth, depth, -40)) {
    /* SCUBA max contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (isBoundary(lastDepth, depth, -105)) {
    /* K350 max contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (depth> -40 && depth <= 0) {
    /* SCUBA depth */
    if (depth >= -2.0) {
      depth = -2.0;
    }
    lastBlue = 255;
    lastRed = 0;
    lastGreen = 165 + int(depth*2);
  } else if (depth> -105 && depth <= -40) {
    /* K350 depth */
    lastBlue = 150 + (40 + int(depth)) * 2 ;
    lastRed = 0;
    lastGreen = 65 + (40 + int(depth));
  } else if (depth < -105 && depth > -210) {
    lastBlue = 0;
    lastRed = 255 + 105 + int(depth);
    lastGreen = 0;
  } else if (depth < -210) {
    lastBlue = (depth - 600) / 23;
    lastGreen = 0;
    lastRed = (depth - 600) / 23;
  } else {
    lastBlue = depth < 0 ? depth / 23 : 0;
    lastGreen = depth > 0 ? int(depth) % 255 : int(depth) %  250;
    lastRed = depth > 0 ? 50 + depth / 15 : 0;
  }
}

/* Depth coloring in bands based on:
 * scuba depth
 * K250 depth
 * K350 depth
 * the rest
 */
void TerrainBuilder::experimentalProcessColorFor(float depth) {
  int contourSpacing = 500;
  int contourZone = int(depth / contourSpacing);
  int oldContourZone = int(lastDepth / contourSpacing);
  if (depth < -10 && contourZone != oldContourZone) {
    /* draw contours */
    lastBlue = 255;
    lastRed = 255;
    lastGreen = 255;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (isBoundary(lastDepth, depth, -40)) {
    /* SCUBA max contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (isBoundary(lastDepth, depth, -75)) {
    /* K250 max depth contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (isBoundary(lastDepth, depth, -105)) {
    /* K350 max depth contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
  } else if (isBoundary(lastDepth, depth, -180)) {
    /* K600 max depth contour. */
    lastBlue = 0;
    lastRed = 255;
    lastGreen = 0;
    forceColorMapCalc = true;
    skipColorMapCount = 2;
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

SoSeparator *TerrainBuilder::getWater() {
  SoSeparator *water = new SoSeparator();
 
  SoMaterial *material = new SoMaterial;
  material->ambientColor.setValue(0.0, 0.0, 0.8);
  material->diffuseColor.setValue(0.0, 0.0, 0.8);
  material->transparency.setValue(0.6);
  material->shininess.setValue(0.6);
  material->emissiveColor.setValue(0.000000, 0.000000, 0.000000);

  water->addChild(material);
  water->addChild(waterCoords);

  SoQuadMesh * mesh = new SoQuadMesh;
  mesh->verticesPerRow = terrainHeight;
  mesh->verticesPerColumn = terrainWidth;

  water->addChild(mesh);
  return water;
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

  // Offset handling, number of columns or rows of data points
  // to skip. 
  int xoffset = xDataPointOffset;
  int yoffset = yDataPointOffset;
  int row_count = -1;
  int col_count = 0;
  Position first, loc;
  bool refSet = false;

  unsigned char *imageMap = new unsigned char[terrainWidth * terrainHeight * 3];

  SbVec3f * points = coords->point.startEditing();
  SbVec2f * texture_points = texture_coords->point.startEditing();
  SbVec3f * normal_points = normals->vector.startEditing();
  SbVec3f * waterSurfacePoints = waterCoords->point.startEditing();

  FILE *hmFile = fopen (filename , "r");

  for (I = 0; I < dataCount; I++)
  {
    fscanf(hmFile, "%f %f %f", &lng, &lat, &zvalue);

    if (current_lat != lat) {
      // We are starting a new row.
      current_lat = lat;
      col_count = terrainWidth - 1 + xoffset;
      row_count++;
    } else {
      col_count--;
    }

    if (lat > maxLat) {
      maxLat = lat;
    }

    if (lat < minLat) {
      minLat = lat;
    }

    if (lng > maxLong) {
      maxLong = lng;
    }

    if (lng < minLong) {
      minLong = lng;
    }

    if (( col_count >= 0 && col_count < terrainWidth ) &&
        ( row_count >= yoffset && row_count < terrainWidth + yoffset)) {
      if (!refSet) {
        // Set first position (everything is should drawn relative to this point.
        first.set_LLA(0.0, 0.0, 0.0, WGS84);
        maxLat = minLat = ref_lat = first_lat = lat;
        maxLong = minLong = ref_long = first_lng = lng;
        refSet = true;
      }                  
      loc.set_LLA(lat - first_lat, lng - first_lng, zvalue, WGS84);

      x = (first.get_x() - loc.get_x()) / scaleFactor;
      y = loc.get_y() / scaleFactor;
      z = loc.get_z() / scaleFactor;

      pointTerrainCount = col_count + (row_count - yoffset) * terrainWidth;
      
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
      loc.set_LLA(lat - first_lat, lng - first_lng, 0.0f, WGS84);

      float seaLevel = (first.get_x() - loc.get_x()) / scaleFactor;

      waterSurfacePoints[pointTerrainCount] = SbVec3f(-z, y, -seaLevel);
      texture_points[pointCount] = SbVec2f(
          col_count * 1.0 / (terrainWidth) * 1.0,
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
  waterCoords->point.finishEditing();
}

/**
 * TODO(irox): Move this into a constructor or something.  It's rather
 * confusing since something need set before this method and others after it.
 */
void TerrainBuilder::initialize() {
  texture = new SoTexture2();
  texture_coords = new SoTextureCoordinate2();
  coords = new SoCoordinate3();
  normals = new SoNormal();
  normal_binding = new SoNormalBinding();
  waterCoords = new SoCoordinate3();

  coords->point.setNum(terrainWidth * terrainHeight);
  texture_coords->point.setNum(terrainWidth * terrainHeight);
  normals->vector.setNum(terrainWidth * terrainHeight);
  normal_binding->value.setValue(SoNormalBinding::PER_VERTEX_INDEXED);
  waterCoords->point.setNum(terrainWidth * terrainHeight);

  colorMap = TerrainBuilder::EXPERIMENTAL_COLOR;
  lastDepth = -999999;
  skipColorMapCount = 0;
  forceColorMapCalc = false;
  yDataPointOffset = 0;
  scaleFactor = 100000;
}

void TerrainBuilder::setScalingFactor(int scale) {
  scaleFactor = scale;
}
void TerrainBuilder::setYOffset(int offset) {
  yDataPointOffset = offset;
}

void TerrainBuilder::setXOffset(int offset) {
  xDataPointOffset = offset;
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

