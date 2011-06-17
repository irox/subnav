bin_PROGRAMS = SoTerrainTest TestPositionServer

SoTerrainTest_SOURCES = SoTerrainTest.cpp \
			TerrainBuilder.cpp \
                        MarkerPin.cpp \
                        nmlPosition_n.cc \
                        SoQtFreeViewer.cpp include/SoQtFreeViewer.h \
                        utils.cpp include/utils.h \
                        include/debug.h
SoTerrainTest_CPPFLAGS = $(AM_CPPFLAGS)
SoTerrainTest_LDFLAGS = -lrcs -lnav -lPrProfiler

TestPositionServer_SOURCES = TestPositionServer.cpp \
			     nmlPosition_n.cc
TestPositionServer_CPPFLAGS = $(AM_CPPFLAGS)
TestPositionServer_LDFLAGS = -lrcs

nmlPosition_n.cc:
	java -jar ../CodeGenCmdLine.jar nmlPosition.h
