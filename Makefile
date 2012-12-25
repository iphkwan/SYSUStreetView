CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all: stitching

stitching: stitching.cpp
	$(CXX) $^ -o $@ $(CFLAGS) $(LIBS)
