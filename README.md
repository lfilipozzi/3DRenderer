Virtual Model
=============

This application produces a Carsim like 3D animation of a vehicle.

Build
-------------

Initialize the submodules:
```shell
git submodule update --recursive --init
```

Build assimp as a static library
```shell
cd lib/assimp
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_ZLIB=ON ..
make
```

Run the following lines to compile the renderer
```shell
cd ../../..
mkdir build && cd build
qmake QMAKE_CFLAGS_ISYSTEM='' ..
make
```

Finally, to export video from the animation, make sure ffmpeg is installed.

Dependencies
-------------

* OpenGL 3.3
* Qt5 with XML patterns module
* assimp
* ffmpeg


