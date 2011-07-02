bin_PROGRAMS = TerrainViewer TestPositionServer

TerrainViewer_SOURCES = TerrainViewer.cpp \
			TerrainBuilder.cpp \
                        MarkerPin.cpp \
                        nmlPosition_n.cc \
                        SoQtFreeViewer.cpp include/SoQtFreeViewer.h \
                        utils.cpp include/utils.h \
                        include/debug.h
TerrainViewer_CPPFLAGS = $(AM_CPPFLAGS)
TerrainViewer_LDFLAGS = -lrcs -lnav -lPrProfiler

TestPositionServer_SOURCES = TestPositionServer.cpp \
			     nmlPosition_n.cc
TestPositionServer_CPPFLAGS = $(AM_CPPFLAGS)
TestPositionServer_LDFLAGS = -lrcs

nmlPosition_n.cc:
	@RCSLIB_PATH@/bin/CodeGen.sh nmlPosition.h
