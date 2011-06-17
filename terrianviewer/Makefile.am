bin_PROGRAMS = SoTerrainTest

SoTerrainTest_SOURCES = SoTerrainTest.cpp \
			TerrainBuilder.cpp \
                        MarkerPin.cpp \
                        nmlPosition_n.cc \
                        SoQtFreeViewer.cpp include/SoQtFreeViewer.h \
                        utils.cpp include/utils.h \
                        include/debug.h
SoTerrainTest_CPPFLAGS = $(AM_CPPFLAGS)
SoTerrainTest_LDFLAGS = -lrcs -lnav -lPrProfiler

nmlPosition_n.cc:
	java -jar ../CodeGenCmdLine.jar nmlPosition.h
