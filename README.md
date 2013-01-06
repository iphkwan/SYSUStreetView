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

## Install And Use PanoTools

Install Dependence

```bash
sudo apt-get install libpano13-2 libpano13-bin
```

Basic steps

```bash
pto_gen -o project.pto *.JPG  ## Generate PTO file
cpfind -o project.pto --multirow --celeste project.pto ## Find Control Points, with celeste to ignore clouds
cpclean -o project.pto project.pto ## Control Point Cleaning
linefind -o project.pto project.pto ## Find Vertical lines
## Optimize position, do photometric optimization, straighten panorama and select suitable output projection
autooptimiser -a -m -l -s -o project.pto project.pto
pano_modify --canvas=AUTO --crop=AUTO -o project.pto project.pto ## Calculate optimal crop and optimal size
pto2mk -o project.mk -p prefix project.pto ## Generate stitching makefile
make -f project.mk all ## Generate final Panorama
```

The final step can be seperated into two basic steps

```bash
nona -m TIFF_m -o project project.pto
enblend -o project.tif project0000.tif project0001.tif ...
```

Then you get the TIFF image. Use `convert xxx.tif xxx.png` to convert it to PNG format.

## Install FreePV

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

## Compile PanoGLView

Install Dependences

```bash
sudo apt-get install wx-common libwxgtk2.8-dev libglew-dev
```

Get sources and compile

```bash
mkdir panoglview
cd panoglview
hg clone http://hugin.hg.sourceforge.net:8000/hgroot/hugin/panoglview panoglview.hg
cd panoglview.hg
hg update
./bootstrap
./configure
make
```

Then you can find `panoglview` in the `src` folder

## Other Open-sources Pano Viewers

* [Interactive Panorama Viewer](http://sourceforge.net/projects/panoramaviewer/?source=recommended)

* [Panini perspective tool](http://sourceforge.net/projects/pvqt/?source=recommended)

## Compile & Run
```
make
./stitching img/*
xdg-open result.jpg
```
