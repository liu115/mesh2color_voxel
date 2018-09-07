CPPFLAGS= -Wall -O2 -std=c++11
SRCDIR=src/
TARGET=$(SRCDIR)mesh2vox.cpp
UTILS=$(SRCDIR)tinyply.cpp $(SRCDIR)triangleCube.cpp $(SRCDIR)polygon_3darea.cpp
#all:
#	g++ $(CPPFLAGS) $(TARGET) $(UTILS)
MEAN:
	g++ $(CPPFLAGS) $(TARGET) $(UTILS) -o mesh2vox_mean
MAX:
	g++ $(CPPFLAGS) -D TAKE_MAX_AREA_COLOR $(TARGET) $(UTILS) -o mesh2vox_max

