CV_CFLAGS = `pkg-config --cflags opencv`
CV_LIBS = `pkg-config --libs opencv`

GL_LIBS = -lGL -lGLU -lglut

all: stitching stitching_detailed view

stitching: stitching.cpp
	$(CXX) $^ -o $@ $(CV_CFLAGS) $(CV_LIBS)

stitching_detailed: stitching_detailed.cpp
	$(CXX) $^ -o $@ $(CV_CFLAGS) $(CV_LIBS)

view: view.cpp
	$(CXX) $^ -o $@ $(GL_LIBS)
