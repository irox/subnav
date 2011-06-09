///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
///
/// \file SoTerrainTest.cpp
/// \author Radek Barto�- xbarto33
/// \date 06.03.2006
///
/// </DL>
//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2006 Radek Barton
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
///////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransparencyType.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <iostream>
#include <cmath>

#include <roam/SoSimpleROAMTerrain.h>
#include <geomipmapping/SoSimpleGeoMipmapTerrain.h>
#include <chunkedlod/SoSimpleChunkedLoDTerrain.h>
#include <profiler/PrProfiler.h>
#include <profiler/SoProfileGroup.h>
#include <profiler/SoProfileSceneManager.h>
#include <SoQtFreeViewer.h>
#include <utils.h>
// TODO(irox): Add check for Position.hpp to configure.ac.
#include <libnav/Position.hpp>
using namespace libnav;

#if defined(__WIN32__) || defined(_WIN32)
  #pragma comment(linker, "/entry:\"mainCRTStartup\"")
  #pragma comment(linker, "/subsystem:\"console\"")
#endif

enum
{
  ID_ALG_BRUAL_FORCE = 0,
  ID_ALG_ROAM = 1,
  ID_ALG_GEO_MIPMAP = 2,
  ID_ALG_CHUNKED_LOD = 3
};

int algorithm = ID_ALG_ROAM;
float animation_time = 30.0f;
float frame_time = 0.04f;
SbBool is_synchronize = FALSE;

/* Change terrain properties by key press callback. */
void terrainCallback(void * userData, SoEventCallback * eventCB)
{
  const SoKeyboardEvent * event =
    reinterpret_cast<const SoKeyboardEvent *>(eventCB->getEvent());

  if (SO_KEY_PRESS_EVENT(event, ENTER))
  {
    switch (algorithm)
    {
      case ID_ALG_ROAM:
      {
        SoSimpleROAMTerrain * terrain = reinterpret_cast<SoSimpleROAMTerrain *>
          (userData);
        terrain->freeze.setValue(!terrain->freeze.getValue());
      }
      break;
      case ID_ALG_GEO_MIPMAP:
      {
        SoSimpleGeoMipmapTerrain * terrain = reinterpret_cast<SoSimpleGeoMipmapTerrain *>
          (userData);
        terrain->freeze.setValue(!terrain->freeze.getValue());
      }
      break;
      case ID_ALG_CHUNKED_LOD:
      {
        SoSimpleChunkedLoDTerrain * terrain = reinterpret_cast<SoSimpleChunkedLoDTerrain *>
          (userData);
        terrain->freeze.setValue(!terrain->freeze.getValue());
      }
      break;
    }
    eventCB->setHandled();
  }
  else if (SO_KEY_PRESS_EVENT(event, PAD_ADD))
  {
    switch (algorithm)
    {
      case ID_ALG_ROAM:
      {
        SoSimpleROAMTerrain * terrain = reinterpret_cast<SoSimpleROAMTerrain *>
          (userData);
        terrain->pixelError.setValue(terrain->pixelError.getValue() + 1);
      }
      break;
      case ID_ALG_GEO_MIPMAP:
      {
        SoSimpleGeoMipmapTerrain * terrain = reinterpret_cast<SoSimpleGeoMipmapTerrain *>
          (userData);
        terrain->pixelError.setValue(terrain->pixelError.getValue() + 1);
      }
      break;
      case ID_ALG_CHUNKED_LOD:
      {
        SoSimpleChunkedLoDTerrain * terrain = reinterpret_cast<SoSimpleChunkedLoDTerrain *>
          (userData);
        terrain->pixelError.setValue(terrain->pixelError.getValue() + 1);
      }
      break;
    }
    eventCB->setHandled();
  }
  else if (SO_KEY_PRESS_EVENT(event, PAD_SUBTRACT))
  {
    switch (algorithm)
    {
      case ID_ALG_ROAM:
      {
        SoSimpleROAMTerrain * terrain = reinterpret_cast<SoSimpleROAMTerrain *>
          (userData);
        if (terrain->pixelError.getValue() > 1)
        {
          terrain->pixelError.setValue(terrain->pixelError.getValue() - 1);
        }
      }
      break;
      case ID_ALG_GEO_MIPMAP:
      {
        SoSimpleGeoMipmapTerrain * terrain = reinterpret_cast<SoSimpleGeoMipmapTerrain *>
          (userData);
        if (terrain->pixelError.getValue() > 1)
        {
          terrain->pixelError.setValue(terrain->pixelError.getValue() - 1);
        }
      }
      break;
      case ID_ALG_CHUNKED_LOD:
      {
        SoSimpleChunkedLoDTerrain * terrain = reinterpret_cast<SoSimpleChunkedLoDTerrain *>
          (userData);
        if (terrain->pixelError.getValue() > 1)
        {
          terrain->pixelError.setValue(terrain->pixelError.getValue() - 1);
        }
      }
      break;
    }
    eventCB->setHandled();
  }
  else if (SO_KEY_PRESS_EVENT(event, F))
  {
    switch (algorithm)
    {
      case ID_ALG_ROAM:
      {
        SoSimpleROAMTerrain * terrain = reinterpret_cast<SoSimpleROAMTerrain *>
          (userData);
        terrain->frustrumCulling.setValue(!terrain->frustrumCulling.getValue());
      }
      break;
      case ID_ALG_GEO_MIPMAP:
      {
        SoSimpleGeoMipmapTerrain * terrain = reinterpret_cast<SoSimpleGeoMipmapTerrain *>
          (userData);
        terrain->frustrumCulling.setValue(!terrain->frustrumCulling.getValue());
      }
      break;
      case ID_ALG_CHUNKED_LOD:
      {
        SoSimpleChunkedLoDTerrain * terrain = reinterpret_cast<SoSimpleChunkedLoDTerrain *>
          (userData);
        terrain->frustrumCulling.setValue(!terrain->frustrumCulling.getValue());
      }
      break;
    }
    eventCB->setHandled();
  }
}

void styleCallback(void * userData, SoEventCallback * eventCB)
{
  SoDrawStyle * style = reinterpret_cast<SoDrawStyle *>(userData);
  const SoKeyboardEvent * event =
    reinterpret_cast<const SoKeyboardEvent *>(eventCB->getEvent());

  if (SO_KEY_PRESS_EVENT(event, L))
  {
    if (style->style.getValue() == SoDrawStyle::FILLED)
    {
      style->style.setValue(SoDrawStyle::LINES);
    }
    else
    {
      style->style.setValue(SoDrawStyle::FILLED);
    }
  }
}

/* Camera animation callback.  */
void cameraTimerCallback(void * userData, SoSensor * sensor)
{
  /* Spatial points for camera movement. */
  static const int CAMERA_POINTS_COUNT = 10;
  static const SbVec3f camera_positions[CAMERA_POINTS_COUNT] =
    {SbVec3f(0.0f, 0.1f, 0.03f), SbVec3f(0.6f, 0.1f, 0.03f),
     SbVec3f(0.6f, 0.1f, 0.03f), SbVec3f(0.6f, 0.6f, 0.03f),
     SbVec3f(0.6f, 0.6f, 0.03f), SbVec3f(0.2f, 0.6f, 0.03f),
     SbVec3f(0.2f, 0.6f, 0.03f), SbVec3f(0.6f, 0.6f, 0.03f),
     SbVec3f(0.6f, 0.6f, 0.03f), SbVec3f(0.6f, 0.1f, 0.03f)};
  static const SbRotation camera_orientations[CAMERA_POINTS_COUNT] =
    {SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -static_cast<float>(M_PI) / 2.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -static_cast<float>(M_PI) / 2.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), static_cast<float>(M_PI) / 2.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), static_cast<float>(M_PI) / 2.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -static_cast<float>(M_PI) / 2.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -static_cast<float>(M_PI) / 2.0f),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -static_cast<float>(M_PI)),
     SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) / 2.0f) *
       SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -static_cast<float>(M_PI))};

  static int frame = 0;
  static SbTime start_time = SbTime::zero();
  SoPerspectiveCamera * camera = reinterpret_cast<SoPerspectiveCamera *>
    (userData);

  if (frame == 0)
  {
    camera->position = camera_positions[0];
    camera->orientation = camera_orientations[0];
  }
  else
  {
    if (frame == 1)
    {
      start_time = SbTime::getTimeOfDay();
    }

    float position = 0.0;
    if (is_synchronize)
    {
      position = ((SbTime::getTimeOfDay() - start_time).getValue() / animation_time)
        * CAMERA_POINTS_COUNT;
    }
    else
    {
      position = ((frame * frame_time) / animation_time) * CAMERA_POINTS_COUNT;
    }

    int point = static_cast<int>(position);
    float ratio = position - point;

    if (point >= CAMERA_POINTS_COUNT - 1)
    {
      SoQt::exitMainLoop();
    }

    camera->position = (1.0f - ratio) * camera_positions[point] +
      ratio * camera_positions[point + 1];
    SbRotation orientation_1 = camera_orientations[point];
    SbRotation orientation_2 = camera_orientations[point + 1];
    camera->orientation = SbRotation::slerp(orientation_1, orientation_2, ratio);
  }

  frame++;
}

void renderCallback(void * _render_area, SoSceneManager * scene_manager)
{
  /* Redraw render area when using custom scene manager. */
  SoQtRenderArea * render_area = reinterpret_cast<SoQtRenderArea *>(_render_area);
  render_area->render();
}

void help()
{
  std::cout << "Usage: SoTerrainTest -h heightmap [-t texture] [-p profile_file] "
    "[-a algorithm] [-A animation_time] [-F frame_time] [-e pixel_error] "
    "[-r triangle_count] [-g tile_size] [-f] [-c] [-v] [-s]" << std::endl;
  std::cout << "\t-h heightmap\t\tImage with input heightmap." <<
    std::endl;
  std::cout << "\t-t texture\t\tImage with terrain texture." << std::endl;
#ifdef PROFILE
  std::cout << "\t-p profile_file\t\tFile for profiling output (default: profile.txt)."
    << std::endl;
#endif
  std::cout << "\t-a algorithm\t\tAlgorithm of terrain visualization. (default: roam)"
    << std::endl;
  std::cout << "\t\tbrutalforce\t\tBrutal force terrain rendering." <<
    std::endl;
  std::cout << "\t\troam\t\t\tROAM algorithm terrain rendeing." << std::endl;
  std::cout << "\t\tgeomipmapping\t\tGeo Mip-Mapping algorithm terrain rendering."
    << std::endl;
  std::cout << "\t-A animation_time\tLength of animation in miliseconds (default: 30 s)."
    << std::endl;
  std::cout << "\t-F frame_time\t\tFrame time in miliseconds (default: 40 ms)."
    << std::endl;
  std::cout << "\t-e pixel_error\t\tDisplay error of rendering in pixels (default: 6)."
    << std::endl;
  std::cout << "\t-r triangle_count\tMaximal number of triangles in triangulation. (default 10.000)"
    << std::endl;
  std::cout << "\t-g tile_size\t\tSize of side of each tile in tile based algorithms. (default: 33)"
    << std::endl;
  std::cout << "\t-f\t\t\tRun application at fullscreen." << std::endl;
  std::cout << "\t-c\t\t\tEnable frustrum culling." << std::endl;
  std::cout << "\t-v\t\t\tRun animation at application start." << std::endl;
  std::cout << "\t-s\t\t\tEnable animation synchronization with time." << std::endl;
}

int main(int argc, char * argv[])
{
  /* Default values of program arguments. */
  char * heightmap_name = NULL;
  char * texture_name = NULL;
  char * profile_name = "profile.txt";
  int triangle_count = 1000;
  int tile_size = 33;
  int pixel_error = 6;
  SbBool is_animation = FALSE;
  SbBool is_full_screen = FALSE;
  SbBool is_frustrum_culling = TRUE;

  /* Get program arguments. */
  int command = 0;
  while ((command = getopt(argc, argv, "h:t:p:a:A:F:e:r:g:fcvs")) != -1)
  {
    switch (command)
    {
      /* Heightmap. */
      case 'h':
      {
        heightmap_name = optarg;
      }
      break;
      /* Texture. */
      case 't':
      {
        texture_name = optarg;
      }
      break;
      /* File for output of profiler. */
      case 'p':
      {
        profile_name = optarg;
      }
      break;
      /* Algorithm. */
      case 'a':
      {
        if (!strcmp(optarg, "roam"))
        {
          algorithm = ID_ALG_ROAM;
        }
        else if (!strcmp(optarg, "geomipmapping"))
        {
          algorithm = ID_ALG_GEO_MIPMAP;
        }
        else if (!strcmp(optarg, "chunkedlod"))
        {
          algorithm = ID_ALG_CHUNKED_LOD;
        }
        else if (!strcmp(optarg, "brutalforce"))
        {
          algorithm = ID_ALG_BRUAL_FORCE;
        }
      }
      break;
      /* Animation time. */
      case 'A':
      {
        int tmp = 0;
        sscanf(optarg, "%d", &tmp);
        animation_time = tmp * 0.001f;
      }
      break;
      /* Animation frame time. */
      case 'F':
      {
        int tmp = 0;
        sscanf(optarg, "%d", &tmp);
        frame_time = tmp * 0.001f;
      }
      break;
      /* Pixel error of rendering. */
      case 'e':
      {
        sscanf(optarg, "%d", &pixel_error);
      }
      break;
      /* Number of triangles in triangulation. */
      case 'r':
      {
        sscanf(optarg, "%d", &triangle_count);
      }
      break;
      /* Tile side size. */
      case 'g':
      {
        sscanf(optarg, "%d", &tile_size);
      }
      break;
      /* Fullscreen. */
      case 'f':
      {
        is_full_screen = TRUE;
      }
      break;
      /* Frustrum culling. */
      case 'c':
      {
        is_frustrum_culling = FALSE;
      }
      break;
      /* Do animation. */
      case 'v':
      {
        is_animation = TRUE;
      }
      break;
      /* Synchronize animation with time. */
      case 's':
      {
        is_synchronize = TRUE;
      }
      break;
      case '?':
      {
        std::cout << "Unknown option!" << std::endl;
        help();
        exit(1);
      }
      break;
    }
  }

  /* Check obligatory arguments. */
  if (heightmap_name == NULL)
  {
    std::cout << "Input height map wasn't specified!" << std::endl;
    help();
    exit(1);
  }

  /* Load heightmap. */
  int mapwidth = 0;
  int mapheight = 0;
  int height = 0;
  int components = 0;
 // unsigned char * heightmap = simage_read_image(heightmap_name, &width,
 //   &height, &components);

  FILE *hmFile = fopen (heightmap_name , "r");

//  fscanf(hmFile, "NCOLS  %d\n", &mapwidth);
//  fscanf(hmFile, "NROWS  %d\n", &mapheight);
  
  int width = 1024 * 4 + 1; //4097; // + 512; //2049; //1025;
  height = width;
//  width--; height--;
  float dummyf;
  // Scan the next few lines but toss the data for now.
//  fscanf(hmFile, "XLLCENTER %f\n", &dummyf);
 // fscanf(hmFile, "YLLCENTER  %f\n", &dummyf);
//  fscanf(hmFile, "CELLSIZE %f\n", &dummyf);
//  fscanf(hmFile, "NODATA_VALUE  %f\n", &dummyf);
//  fscanf(hmFile, "%*[ \n\t]%f", &dummyf);
  float dummyf2;
//  fscanf(hmFile, "%*[ \n\t]%f", &dummyf2);

//  std::cout << "Width = " << width << " , height = " << height << " first val = " << dummyf << "second val = " << dummyf2 << std::endl;
//  exit(0);

//  if (heightmap == NULL)
//  {
//    std::cout << "Error loading height map " << heightmap_name << "!"
//      << std::endl;
//    exit(1);
//  }

  PR_INIT_PROFILER();

  /* Set environment variables. */
  //putenv("IV_SEPARATOR_MAX_CACHES=0");
  putenv("COIN_SHOW_FPS_COUNTER=1");
  //putenv("COIN_AUTO_CACHING=0");

  /* Create window. */
  QWidget * window;
  if ((window = SoQt::init(argc, argv, "SoTerrain Test Application")) == NULL)
  {
    exit(1);
  }

  /* Initialization of custom Inventor classes. */
  SoSimpleROAMTerrain::initClass();
  SoSimpleGeoMipmapTerrain::initClass();
  SoSimpleChunkedLoDTerrain::initClass();
  SoProfileGroup::initClass();

  // Create slightly opaque blue cube to be the sea/water.
 
  /* Create scene graph. */
  SoProfileGroup * root = new SoProfileGroup();
  SoEventCallback * style_callback = new SoEventCallback();
  SoPerspectiveCamera * camera = new SoPerspectiveCamera();
  SoDrawStyle * style = new SoDrawStyle();
  SoDirectionalLight * light = new SoDirectionalLight();
  SoSeparator * separator = new SoSeparator();
  SoSeparator * waterSep = new SoSeparator();

  SoEventCallback * terrain_callback = new SoEventCallback();
  SoTexture2 * texture = new SoTexture2();
  SoTextureCoordinate2 * texture_coords = new SoTextureCoordinate2();
  SoCoordinate3 * coords = new SoCoordinate3();
  SoNormal * normals = new SoNormal();
  SoNormalBinding * normal_binding = new SoNormalBinding();

  style_callback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
    styleCallback, style);
  light->direction.setValue(-0.5f, 0.5f, -1.0f);
//  texture->filename.setValue(texture_name);
  coords->point.setNum(width * height);
  int imageWidth = 4096;
  int imageHeight = 4096;
  texture_coords->point.setNum((imageWidth + 1 ) * (imageHeight + 1));
  normals->vector.setNum(width * height);
  normal_binding->value.setValue(SoNormalBinding::PER_VERTEX_INDEXED);

  /* Create heightmap. */
  SbVec3f * points = coords->point.startEditing();
  SbVec2f * texture_points = texture_coords->point.startEditing();
  SbVec3f * normal_points = normals->vector.startEditing();
  SoSFImage * image = new SoSFImage();
 // SbVec2s *imageSize= new SbVec2s(4096, 2048 /*4095*/ /*width - 2, height -1*/);
  SbVec2s imageSize(imageWidth, imageHeight);
 // float zdat[4801 * 7201];
  unsigned char *imageMap = new unsigned char[4098 * 4097 * 3];
  int pointCount = 0; //width * height - 1;
  int pointTerrainCount = 0;
  float x,y;
  float firsty = 0;
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
	
  for (I = 0; I < 4801 * 4801; I++)
  {
    fscanf(hmFile, "%f %f %f", &lng, &lat, &zvalue);

    if (current_lat == 999) {
      // Set first position (everything is should drawn relative to this point.
      first.set_LLA(0.0, 0.0, 0.0, WGS84);
      first_lat = lat;
      first_lng = lng;
    }

    if (current_lat != lat) {
      // We are starting a new row.
      current_lat = lat;
      col_count = width - 1; //0;
      row_count++;
 //std::cout <<  pointTerrainCount << " " << zvalue << " "  << (loc.get_x() - first.get_x()) << " " << (loc.get_y() - first.get_y()) << " " << (loc.get_z() - first.get_z()) << std::endl;
    } else {
      //col_count++;
      col_count--;
    }
    
//      std::cout <<  pointTerrainCount << " col = " << col_count << "  row = " << row_count << std::endl;

    if (( col_count >= 0 /*col_count < width */) && ( row_count < width)) {
      loc.set_LLA(lat - first_lat, lng - first_lng, zvalue, WGS84);

      x = (first.get_x() - loc.get_x()) / 100000;
      y = loc.get_y() / 100000;
      z = loc.get_z() / 100000   ;
/*      x = (loc.get_x() - first.get_x()) / 100000;
      y = (loc.get_y() - first.get_y()) / 100000;
      z = (loc.get_z() - first.get_z()) / 100000;
*/

/*      x = (first.get_x() - loc.get_x()) / 100000;
      
      y = (first.get_y() - loc.get_y()) / 100000;
      z = (first.get_z() - loc.get_z()) / 100000;
*/
//      std::cout <<  pointTerrainCount << " x = " << x << "  y = " << y << "   z = " << z << std::endl;
/*      float zscale = 0.00002f;
      int column = (width - I % mapwidth - 1);
      int row = (I / mapwidth - (mapheight - height)) * width;
      int imageColumn = col_count; 
      int imageRow = (I / mapwidth - (mapheight - height)) * imageWidth;*/
      pointTerrainCount = col_count + row_count * width;
    //  std::cout <<  pointTerrainCount << " col: " << column << " row: " << row << " " << I / mapwidth << " " << (mapheight - height -1) << " " << width - I % mapwidth << " " << width - I % mapwidth << std::endl;
     
     // Calculate pixel colo(u)r...
     int red, green, blue;

     if (zvalue > -150 && zvalue < 0) {
       blue = int(zvalue / 1.33);
       green = 0;
       red = 0;
     } else if (zvalue < -300 && zvalue > -600) {
       blue = 0;// 225; //zvalue < 0 ? zvalue / 23 : 0;
       green = int(zvalue / 1.33);//zvalue < 0 ? zvalue / 23 : 0;
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
    // imageIndex = (row * width + column) * 3;
 //    imageIndex = (row + column ) * 3;
  // imageIndex = imageRow + imageColumn;
//   std::cout <<  imageIndex << " " << imageRow << " " << imageColumn<<std::endl;
   if ( col_count < 5 ) {
       imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 000;

   } else if (imageIndex % imageWidth < 1) {
    /*   imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 255;
*/
     } else if (imageIndex < 6 * imageWidth) {
       imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 0;
       imageMap[imageIndex++] = 0;
     } else if (imageIndex % imageWidth < 6) {
       imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 255;
       imageMap[imageIndex++] = 255;
     } else if (I % (mapwidth )  < imageWidth + 1)  {
       imageMap[imageIndex++] = red;
       imageMap[imageIndex++ /*+ 1*/] = green;
       imageMap[imageIndex++ /* + 2*/] = blue;
     } else {
       imageMap[imageIndex++] = 0;
       imageMap[imageIndex++] = 0;
       imageMap[imageIndex++] = 255;
     }
 
  //   imageIndex++;
     //  std::cout << imageIndex << " " << imageColumn << " " << imageRow << std::endl;
        

      points[pointTerrainCount] = SbVec3f( 3.79-z, y, -x );
      texture_points[pointCount] = SbVec2f((1-y/3.79087 ), 1-z/3.77219  /*1 - x, 1 - (y - firsty)*/);
      pointCount++;
    }
  }
  
  texture->image.setValue(imageSize, 3, imageMap, SoSFImage::COPY); //SoSFImage::CopyPolicy::COPY); //NULL);
  std::cout << I << "/" << pointCount << " x y z = (" << x << ", " << y << ", " << z << ")" << std::endl;
// texture->image = image;
 // width = 1025;

  /* Compute inner normals. */
  for (int Y = 1; Y < (height - 1); ++Y)
  {
    for (int X = 1; X < (width - 1); ++X)
    {
      int index = Y * width + X;
      SbVec3f normal = SbVec3f(0.0f, 0.0f, 0.0f);

      normal += (points[index - 1] - points[index]).cross(points[index
        - width] - points[index]);
      normal += (points[index - width] - points[index]).cross(points[index
        - width + 1] - points[index]);
      normal += (points[index - width + 1] - points[index]).cross(points[index
        + 1] - points[index]);
      normal += (points[index + 1] - points[index]).cross(points[index
        + width] - points[index]);
      normal += (points[index + width] - points[index]).cross(points[index
        + width - 1] - points[index]);
      normal += (points[index + width - 1] - points[index]).cross(points[index
        - 1] - points[index]);
      normal.normalize();
      normal_points[index] = normal;
    }
  }

  /* Compute normals at top and bottom border. */
  for (int X = 1; X < (width - 1); ++X)
  {
    int index_1 = X;
    int index_2 = (height - 1) * width + X;
    SbVec3f normal_1 = SbVec3f(0.0f, 0.0f, 0.0f);
    SbVec3f normal_2 = SbVec3f(0.0f, 0.0f, 0.0f);

    /* Top border. */
    normal_1 += (points[index_1 + 1] - points[index_1]).cross(points[index_1
      + width] - points[index_1]);
    normal_1 += (points[index_1 + width] - points[index_1]).cross(points[index_1
      + width - 1] - points[index_1]);
    normal_1 += (points[index_1 + width - 1] - points[index_1]).cross(points[index_1
      - 1] - points[index_1]);

    /* Bottom border. */
    normal_2 += (points[index_2 - 1] - points[index_2]).cross(points[index_2
      - width] - points[index_2]);
    normal_2 += (points[index_2 - width] - points[index_2]).cross(points[index_2
      - width + 1] - points[index_2]);
    normal_2 += (points[index_2 - width + 1] - points[index_2]).cross(points[index_2
      + 1] - points[index_2]);

    normal_1.normalize();
    normal_2.normalize();
    normal_points[index_1] = normal_1;
    normal_points[index_2] = normal_2;
  }

  /* Compute normals at left and right border. */
  for (int Y2 = 1; Y2 < (height - 1); ++Y2)
  {
    int index_1 = Y2 * width;
    int index_2 = index_1 + width - 1;
    SbVec3f normal_1 = SbVec3f(0.0f, 0.0f, 0.0f);
    SbVec3f normal_2 = SbVec3f(0.0f, 0.0f, 0.0f);

    /* Left border. */
    normal_1 += (points[index_1 - width] - points[index_1]).cross(points[index_1
      - width + 1] - points[index_1]);
    normal_1 += (points[index_1 - width + 1] - points[index_1]).cross(points[index_1
      + 1] - points[index_1]);
    normal_1 += (points[index_1 + 1] - points[index_1]).cross(points[index_1
      + width] - points[index_1]);

    /* Right border. */
    normal_2 += (points[index_2 - 1] - points[index_2]).cross(points[index_2
      - width] - points[index_2]);
    normal_2 += (points[index_2 + width] - points[index_2]).cross(points[index_2
      + width - 1] - points[index_2]);
    normal_2 += (points[index_2 + width - 1] - points[index_2]).cross(points[index_2
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
  normal = (points[index + 1] - points[index]).cross(points[index + width]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  index = (height * width) - 1;
  normal = (points[index - 1] - points[index]).cross(points[index - width]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  index = (height - 1) * width;
  normal = (points[index - width] - points[index]).cross(points[index - width + 1]
    - points[index]);
  normal += (points[index - width + 1] - points[index]).cross(points[index + 1]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  index = width - 1;
  normal += (points[index + width] - points[index]).cross(points[index + width - 1]
    - points[index]);
  normal += (points[index + width - 1] - points[index]).cross(points[index - 1]
    - points[index]);
  normal.normalize();
  normal_points[index] = normal;

  coords->point.finishEditing();
  texture_coords->point.finishEditing();
  normals->vector.finishEditing();
//  simage_free_image(heightmap);

  // Create slightly opaque blue cube to be the sea/water.
  SoMaterial *material = new SoMaterial;
  material->ambientColor.setValue(0.0, 0.0, 0.8);
  material->diffuseColor.setValue(0.0, 0.0, 0.8);
  material->transparency.setValue(0.7);
  material->shininess.setValue(0.6);
  material->emissiveColor.setValue(0.000000, 0.000000, 0.000000);

  waterSep->addChild(new SoTexture2());

  waterSep->addChild(material);
  SoTransparencyType *transType = new SoTransparencyType();
  transType->value = SoTransparencyType::BLEND;
  waterSep->addChild(transType);
  SoTransform *transform = new SoTransform;
  transform->translation.setValue(0.5, 01.25, -0.5);
  waterSep->addChild(transform);

  SoCube *water = new SoCube();
  water->width  = 1.0;
  water->height = 1.0f;
  water->depth  = 1.0f;
 
  waterSep->addChild(water);
 
  /* Connect scene graph nodes. */
  root->ref();
  root->addChild(style);
	  root->addChild(separator);
	  separator->addChild(terrain_callback);
	  separator->addChild(style_callback);
	  separator->addChild(camera);
	  separator->addChild(light);
  //        separator->addChild(waterSep);
	  separator->addChild(texture);
	  separator->addChild(texture_coords);
	  separator->addChild(coords);
	  separator->addChild(normals);
	  separator->addChild(normal_binding);

	  switch (algorithm)
	  {
	    case ID_ALG_ROAM:
	    {
	      SoSimpleROAMTerrain * terrain = new SoSimpleROAMTerrain();
	      terrain->mapSize.setValue( /*4096 * 2 + 1 );/ */width);
	      terrain->pixelError.setValue(pixel_error);
	      terrain->triangleCount.setValue(triangle_count);
	      terrain->frustrumCulling.setValue(is_frustrum_culling);
	      terrain_callback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
		terrainCallback, terrain);
	      separator->addChild(terrain);
	    }
	    break;
	    case ID_ALG_GEO_MIPMAP:
	    {
	      SoSimpleGeoMipmapTerrain * terrain = new SoSimpleGeoMipmapTerrain();
	      terrain->mapSize.setValue(width);
	      terrain->tileSize.setValue(tile_size);
	      terrain->pixelError.setValue(pixel_error);
	      terrain_callback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
		terrainCallback, terrain);
	      separator->addChild(terrain);
	    }
	    break;
	    case ID_ALG_CHUNKED_LOD:
	    {
	      SoSimpleChunkedLoDTerrain * terrain = new SoSimpleChunkedLoDTerrain();
	      terrain->mapSize.setValue(width);
	      terrain->tileSize.setValue(tile_size);
	      terrain->pixelError.setValue(pixel_error);
	      terrain_callback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
		terrainCallback, terrain);
	      separator->addChild(terrain);
	    }
	    break;
	    case ID_ALG_BRUAL_FORCE:
	    {
	      SoIndexedTriangleStripSet * terrain = new SoIndexedTriangleStripSet();

	      /* Create terrain heightmap vertices indices. */
	      terrain->coordIndex.setNum((height - 1) * ((2 * width) + 1));
	      int * indices = terrain->coordIndex.startEditing();

      int I = 0;
      for (int Y = 0; Y < (height - 1); Y++)
      {
        for (int X = 0; X < width; X++)
        {
          indices[I++] = (Y * width) + X;
          indices[I++] = ((Y + 1) * width) + X;
        }
        indices[I++] = -1;
      }
      terrain->coordIndex.finishEditing();
      separator->addChild(terrain);
    }
    break;
  }
  separator->addChild(waterSep);
  /* Setup camera and render area. */
  SoQtFreeViewer * render_area = new SoQtFreeViewer(window);
  SoSceneManager * scene_manager = new SoSceneManager();
  render_area->setHeadlight(TRUE /*FALSE*/);
  render_area->setSceneManager(scene_manager);
  scene_manager->setRenderCallback(renderCallback, render_area);
  scene_manager->activate();
  camera->viewAll(root, render_area->getViewportRegion());
  camera->nearDistance.setValue(0.01f);
  render_area->setSceneGraph(root);
  render_area->setTitle("SoTerrain Test Application");
  render_area->setSize(SbVec2s(640, 480));
  render_area->show();
  render_area->setFullScreen(is_full_screen);


  /* Run animation or set camera position and orientation. */
  SoTimerSensor * camera_timer = NULL;
  if (is_animation)
  {
    camera_timer = new SoTimerSensor(cameraTimerCallback,
      camera);
    camera_timer->setInterval(frame_time);
    camera_timer->schedule();
  }
  else
  {
    camera->position.setValue(3.79 - z, y, x * 2);
   // camera->orientation.setValue(-0.452279f, 0.426091f, 0.537269f, -0.570291f);
  }

  /* Run application. */
  SoQt::show(window);
  SoQt::mainLoop();
  SoQt::done();

  PR_PRINT_RESULTS(profile_name);

  /* Free memory. */
  root->unref();
  delete camera_timer;
  delete render_area;

  return EXIT_SUCCESS;
}
