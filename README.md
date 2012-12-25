SYSUStreetView
==============

Streetview of SYSU based on OpenCV stitcher.

>> No voice but big deal.

### INSTALL OPENCV

Install dependences

```bash
sudo apt-get install cmake build-essential libgtk2.0-dev libgtk2.0-0 \
    libavcodec-dev libavformat-dev libjpeg62-dev libtiff4-dev \
    libswscale-dev libjasper-dev libgstreamer-0.10-0 \
    libgstreamer-0.10-dev gstreamer0.10-tools gstreamer0.10-plugins-base \
    libgstreamer-plugins-base0.10-dev gstreamer0.10-plugins-good \
    gstreamer0.10-plugins-ugly gstreamer0.10-plugins-bad gstreamer0.10-ffmpeg 
```

If you get errors while compiling, install the dependences by yourself.

Compile OpenCV. This may take a long time. Be patient.

```bash
mkdir build
cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=true -DWITH_OPENNI=true -DWITH_OPENGL=true
make
sudo make install
```

### Compile & Run
```
make
./stitching img/*
xdg-open result.jpg
```
