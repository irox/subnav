SHELL=/bin/sh

PROGRAM=SubUI

CXX=g++
LD=g++

CFLAGS=-DDEBUG -g -ggdb -I/home/irox/install/freeglut/include/ -I/home/irox/Source/kalman/kalman/

CXXFLAGS=$(CFLAGS) `soxt-config --cppflags` -I/home/irox/Source/kalman/kalman/
LDFLAGS=-g `soxt-config --ldflags` `soxt-config --libs` -L/home/irox/install/freeglut/lib/ -lGL


all: $(PROGRAM) insserver


.SUFFIXES: .cpp .o

$(PROGRAM): main.o subHull.o subHullMesh.o os5500p.o
	$(LD) $(LDFLAGS) $^ -o $(PROGRAM)

insserver: ins6DofKalman.o insServer.o /home/irox/Source/kalman/kalman/kstatics.o os5500p.o
	$(LD) $(LDFLAGS) -lkalman -L/home/irox/Source/kalman/kalman $^ -o insserver

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<


clean:
	rm -f $(PROGRAM)
	rm -f *.o
	rm -f *~
