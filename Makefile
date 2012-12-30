CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all: stitching stitching_detailed

stitching: stitching.cpp
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)

stitching_detailed: stitching_detailed.cpp
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)
