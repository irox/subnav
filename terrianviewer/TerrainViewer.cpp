///////////////////////////////////////////////////////////////////////////////
//  TerrainViewer
///////////////////////////////////////////////////////////////////////////////
///
//
// This file originally started life as SoTerrainTest.cpp before some heavy
// modification.  Leaving in original author name a copyright notice.
//
// Ian Roxborough.
//
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
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/sensors/SoIdleSensor.h>

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

#include <TerrainBuilder.h>
#include <MarkerPin.h>
#include <rcs.hh>
#include <nmlPosition.h>
#include <map>

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

// Pin marker long and lat.
float marker_long = -124.0;
float marker_lat  = 40.0;

// Map reference long and lat
// (The first point drawn).
float ref_long = 0.0;
float ref_lat = 0.0;

// Global scaling factor.
int scaleFactor;

MarkerPin *vesselMarker;
SoCoordinate3 *vesselTrackCoords;
SoLineSet *vesselTrack;

std::map<std::string, MarkerPin*> markerMap;

/* Callback for moving the marker pin around. */
void markerCallback(void *userData,  SoEventCallback * eventCB) {
  const SoKeyboardEvent * event =
      reinterpret_cast<const SoKeyboardEvent *>(eventCB->getEvent());

  if (SO_KEY_PRESS_EVENT(event, NUMBER_1)) {
    marker_long = marker_long - 0.001;
  } else if (SO_KEY_PRESS_EVENT(event, NUMBER_2)) {
    marker_long = marker_long + 0.001;
  } else if (SO_KEY_PRESS_EVENT(event, NUMBER_3)) {
    marker_lat = marker_lat - 0.001;
  } else if (SO_KEY_PRESS_EVENT(event, NUMBER_4)) {
    marker_lat = marker_lat + 0.001;
  } else {
    return;
  }
  
  MarkerPin *markerPin = reinterpret_cast<MarkerPin *> (userData);
  markerPin->setLocation(marker_lat, marker_long);
}

void timerSensorCallback(void *userData,  SoSensor *sensor) {
  SoIdleSensor *idleSensor = reinterpret_cast<SoIdleSensor *> (userData);
  idleSensor->schedule();
}

/**
 * Update marker position based on name.  If the marker doesn't
 * exist, create it.
 */
void updateMarkerPosition(SoSeparator *markers, POSITIONDATA_MSG *pos_msg) { 
  MarkerPin *m;
  std::map<std::string, MarkerPin*>::iterator itr = markerMap.find(std::string(pos_msg->name));
  if (itr == markerMap.end()) {
    m = new MarkerPin();
    m->setScalingFactor(scaleFactor);
    m->setLabel(std::string(pos_msg->name).c_str());
    m->setReferencePosition(ref_lat, ref_long);
    m->setLocation(pos_msg->lattitude, pos_msg->longitude);

    markers->addChild(m->getSoMarker());
    markerMap.insert(std::make_pair(std::string(pos_msg->name), m));
  } else {
    m = itr->second;
  }
  m->setLocation(pos_msg->lattitude, pos_msg->longitude);
}

/**
 *  * Process NML position messages and update markers if needed.
 *   */
void updatePositionCallback(void *userData,  SoSensor *sensor) {
  SoSeparator * markers = reinterpret_cast<SoSeparator *> (userData);

  POSITIONDATA_MSG *pos_msg;
  NML positiondata_nml(ex_format, "position_buf1", "TerrainViewer", "../config.nml");
  switch(positiondata_nml.read()) {
    case -1:

      std::cout << "An RCS communication error has occurred." << std::endl;
      break;
    case 0:
      /* Same message as the last time. */
      break;
    case POSITIONDATA_MSG_TYPE:
      pos_msg = (POSITIONDATA_MSG*) positiondata_nml.get_address();
      updateMarkerPosition(markers, pos_msg);
      break;
  }
}

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

void renderCallback(void * _render_area, SoSceneManager * scene_manager)
{
  /* Redraw render area when using custom scene manager. */
  SoQtRenderArea * render_area = reinterpret_cast<SoQtRenderArea *>(_render_area);
  render_area->render();
}

void help()
{
  std::cout << "Usage: TerrainViewer -h xyz_data_file "
    "[-a algorithm] [-e pixel_error] "
    "[-r triangle_count] [-g tile_size] [-f] [-c]" << std::endl;
  std::cout << "\t-h heightmap\t\tTerrain data, in the format \"lat long depth\"." <<
    std::endl;
  std::cout << "\t-x x-offset\t\tNumber of columns of data to skip (from the left)." << std::endl;
  std::cout << "\t-y y-offset\t\tNumber of rows of data to skip (from the top)." << std::endl;
  std::cout << "\t-s size\t\tSize of the plotted map." << std::endl;
  std::cout << "\t-S scale\t\tScaling factor (default: 100000)." << std::endl;
  std::cout << "\t-a algorithm\t\tAlgorithm of terrain visualization. (default: roam)"
    << std::endl;
  std::cout << "\t\tbrutalforce\t\tBrutal force terrain rendering." <<
    std::endl;
  std::cout << "\t\troam\t\t\tROAM algorithm terrain rendeing." << std::endl;
  std::cout << "\t\tgeomipmapping\t\tGeo Mip-Mapping algorithm terrain rendering."
    << std::endl;
  std::cout << "\t-e pixel_error\t\tDisplay error of rendering in pixels (default: 6)."
    << std::endl;
  std::cout << "\t-r triangle_count\tMaximal number of triangles in triangulation. (default 10.000)"
    << std::endl;
  std::cout << "\t-g tile_size\t\tSize of side of each tile in tile based algorithms. (default: 33)"
    << std::endl;
  std::cout << "\t-f\t\t\tRun application at fullscreen." << std::endl;
  std::cout << "\t-c\t\t\tEnable frustrum culling." << std::endl;
}

int main(int argc, char * argv[])
{
  /* Default values of program arguments. */
  char * heightmap_name = NULL;
  int triangle_count = 1000;
  int tile_size = 33;
  int pixel_error = 6;
  int edgeSize = 4096;
  int xOffset = 0;
  int yOffset = 0;

  SbBool is_full_screen = FALSE;
  SbBool is_frustrum_culling = TRUE;

  /* Get program arguments. */
  int command = 0;
  while ((command = getopt(argc, argv, "h:x:y:s:S:a:e:r:g:fc")) != -1)
  {
    switch (command)
    {
      /* Heightmap. */
      case 'h':
      {
        heightmap_name = optarg;
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
      /* Size of the plot map, in data points a long one edge (map is a square). */
      case 's':
      {
        sscanf(optarg, "%d", &edgeSize);
      }
      /* Scaling factor. */
      case 'S':
      {
        sscanf(optarg, "%d", &scaleFactor);
      }
      break;
      /* X offset in data points. */
      case 'x':      {
        sscanf(optarg, "%d", &xOffset);
      }
      break;
      /* Y offset in data points. */
      case 'y':      {
        sscanf(optarg, "%d", &yOffset);
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
  int height = 0;

  // SoTerrain only seems to handle size which
  // are multiples of 1024.  The +1 maybe due
  // to some sloppiness some later in the code
  // (i.e. "<" vs "<=" or something).
  int width = edgeSize +1; // 1024 + 1;// 1024 * 4 + 1;
  height = width;

  PR_INIT_PROFILER();

  /* Set environment variables. */
  //putenv("IV_SEPARATOR_MAX_CACHES=0");
  putenv((char*)"COIN_SHOW_FPS_COUNTER=1");
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

  /* Create scene graph. */
  SoProfileGroup *root = new SoProfileGroup();
  SoEventCallback *style_callback = new SoEventCallback();
  SoPerspectiveCamera *camera = new SoPerspectiveCamera();
  SoDrawStyle *style = new SoDrawStyle();
  SoDirectionalLight *light = new SoDirectionalLight();
  SoSeparator *separator = new SoSeparator();
  SoSeparator *terrainSeparator = new SoSeparator();
  SoSeparator *markers = new SoSeparator();

  // not sure turning off Culling here is having any effect.
  terrainSeparator->pickCulling.setValue(SoSeparator::OFF);
  terrainSeparator->renderCulling.setValue(SoSeparator::OFF);
 
  SoEventCallback *terrain_callback = new SoEventCallback();

  /* Use the TerrainBuilder class to generating the various components. */
  TerrainBuilder terrainBuilder;
  terrainBuilder.setHeight(height);
  terrainBuilder.setWidth(width);
  terrainBuilder.initialize();
  terrainBuilder.setScalingFactor(scaleFactor);
  terrainBuilder.setYOffset(yOffset);
  terrainBuilder.setXOffset(xOffset);

  terrainBuilder.loadXYZFile(heightmap_name, 4801 * 4801);

  SoTexture2 *texture = terrainBuilder.getTexture();
  SoTextureCoordinate2 *texture_coords = terrainBuilder.getTextureCoordinates();
  SoCoordinate3 *coords = terrainBuilder.getMapCoordinates();
  SoNormal *normals = terrainBuilder.getNormals();

  SoNormalBinding *normal_binding = new SoNormalBinding();

  ref_long = terrainBuilder.getRefLong();
  ref_lat  = terrainBuilder.getRefLat();

  style_callback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
    styleCallback, style);
  light->direction.setValue(-0.5f, 0.5f, -1.0f);
  normal_binding->value.setValue(SoNormalBinding::PER_VERTEX_INDEXED);

  // Create the push pin marker.
  marker_lat = 37.753;
  marker_long = -122.440;
  SoResetTransform *resetForTerrain = new SoResetTransform();

  MarkerPin *marker = new MarkerPin();
  marker->setScalingFactor(scaleFactor);
  marker->setReferencePosition(ref_lat, ref_long);
  marker->setLabel("San Francisco");
  marker->setLocation(marker_lat, marker_long);

  SoEventCallback * marker_callback = new SoEventCallback();
   marker_callback->addEventCallback(
       SoKeyboardEvent::getClassTypeId(),
       markerCallback,
       marker);

  vesselMarker = new MarkerPin();
  vesselMarker->setScalingFactor(scaleFactor);
  vesselMarker->setReferencePosition(ref_lat, ref_long);
  vesselMarker->setLabel("Vessel");
  vesselMarker->setLocation(37.5, -122.0);

  vesselTrack = new SoLineSet();
  vesselTrackCoords = new SoCoordinate3();
  /* Connect scene graph nodes. */
  root->ref();
  root->addChild(style);
  root->addChild(separator);
  separator->addChild(terrain_callback);
  separator->addChild(style_callback);
  separator->addChild(marker_callback);
  separator->addChild(camera);
  separator->addChild(light);
  separator->addChild(terrainSeparator);
  terrainSeparator->addChild(resetForTerrain);
  terrainSeparator->addChild(texture);
  terrainSeparator->addChild(texture_coords);
  terrainSeparator->addChild(coords);
  terrainSeparator->addChild(normals);
  terrainSeparator->addChild(normal_binding);
  // Disable water drawning due to performance issues.
  // separator->addChild(terrainBuilder.getWater());
  separator->addChild(markers);
  separator->addChild(marker->getSoMarker());
  separator->addChild(vesselTrackCoords);
  separator->addChild(vesselTrack);

  switch (algorithm)
  {
    case ID_ALG_ROAM:
    {
      SoSimpleROAMTerrain * terrain = new SoSimpleROAMTerrain();
      terrain->mapSize.setValue(width);
      terrain->pixelError.setValue(pixel_error);
      terrain->triangleCount.setValue(triangle_count);
      terrain->frustrumCulling.setValue(is_frustrum_culling);
      terrain_callback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
	terrainCallback, terrain);
      terrainSeparator->addChild(terrain);
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
      terrainSeparator->addChild(terrain);
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
      terrainSeparator->addChild(terrain);
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
      terrainSeparator->addChild(terrain);
    }
    break;
  }

//  terrainSeparator->addChild(terrainBuilder.getWater());

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
  render_area->setTitle("Terrain Viewer Application");
  render_area->setSize(SbVec2s(640, 480));
  render_area->show();
  render_area->setFullScreen(is_full_screen);

  camera->position.setValue(0 , 0, 1);

  // Use a SoIdleSensor to update marker position, this is because
  // we can't add new markers when the tree is still being traversed
  // (only when idle).
  SoIdleSensor * positionUpdater = new SoIdleSensor();
  positionUpdater->setFunction(updatePositionCallback);
  positionUpdater->setData(markers);

  // Use a SoTimeSensor to periodically schedule
  // the SoIdleSensor to update marker positions.
  SoTimerSensor * timerSensor = new SoTimerSensor(timerSensorCallback, root);
  timerSensor->setInterval(0.2f);
  timerSensor->setData(positionUpdater);
  timerSensor->schedule();

  /* Run application. */
  SoQt::show(window);
  SoQt::mainLoop();
  SoQt::done();

//  PR_PRINT_RESULTS(profile_name);

  /* Free memory. */
  root->unref();
  delete positionUpdater;
  delete render_area;

  return EXIT_SUCCESS;
}
