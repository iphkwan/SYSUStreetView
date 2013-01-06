SYSUStreetView
==============

Streetview of SYSU based on OpenCV stitcher.

> No voice but big deal.

## INSTALL OPENCV

Install dependences

```bash
sudo apt-get install cmake build-essential libgtk2.0-dev libgtk2.0-0 \
    libavcodec-dev libavformat-dev libjpeg62-dev libtiff4-dev \
    libswscale-dev libjasper-dev libgstreamer-0.10-0 \
    libgstreamer-0.10-dev gstreamer0.10-tools gstreamer0.10-plugins-base \
    libgstreamer-plugins-base0.10-dev gstreamer0.10-plugins-good \
    gstreamer0.10-plugins-ugly gstreamer0.10-plugins-bad gstreamer0.10-ffmpeg \
    libopenexr-dev python-dev python-numpy libtbb-dev libfaac-dev \
    libxvidcore-dev libx264-dev sphinx-common libv4l-dev libdc1394-22-dev
```

If you get errors while compiling, install the dependences by yourself.

Get the OpenCV sources

```bash
git submodule update
```

Modify `ENABLE_LOG` in  `modules/stitching/include/opencv2/stitching/detail/util.hpp` line 49 to `1`
```cpp
#define ENABLE_LOG 1
```

Compile OpenCV. This may take a long time. Be patient.

```bash
mkdir build
cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON \
    -DWITH_OPENNI=ON -DWITH_OPENGL=ON
make
sudo make install
```

Compile FreePV (Free Panorama Viewer)

```bash
cd freepv
tar -xvf freepv-0.3.0.tar.gz
patch -p0 < freepv-0.3.0-ubuntu10.04lts-p1.patch
cd freepv-0.3.0
cmake .
make && sudo make install
```

Then run `freepv-glut` or `freepv-glx` can open the FreePV GUI. Example

```bash
freepv-glut img/1上-9下.png
```

## Compile & Run
```
make
./stitching img/*
xdg-open result.jpg
```
