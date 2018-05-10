CPPFLAGS= -Wall -O2 -std=c++11
TARGET=mesh2vox.cpp
UTILS=tinyply.cpp triangleCube.cpp polygon_3darea.cpp
#all:
#	g++ $(CPPFLAGS) $(TARGET) $(UTILS)
MEAN:
	g++ $(CPPFLAGS) $(TARGET) $(UTILS) -o mesh2vox_mean
MAX:
	g++ $(CPPFLAGS) -D TAKE_MAX_AREA_COLOR $(TARGET) $(UTILS) -o mesh2vox_max

