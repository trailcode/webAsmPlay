Currently a handful of C++ libraries have been compiled with Emscripten in this project to run in the browser
and run natively. The idea is to test a cross platform development environment where an application
is developed and debugged both as a web application and a native application.
Another goal is to discover the limitations and capabilities of Emscripten web application development.
Cross platform development is achieved using cmake.

### Dependencies

Unfortunately during development notes on build depends were not tracked. Development is currently
being done on OSX. Macports (https://www.macports.org/) and Brew (https://brew.sh/) is being used to
install third party dependencies. C++ 14 is being used. Currently emscripten is using clang 5.0 which
does not fully support c++ 17. On windows it is recommended to use scoop (https://scoop.sh) to install third party requirements

* Cmake: https://cmake.org
* Emscripten: http://kripken.github.io/emscripten-site
* Glfw3: https://www.glfw.org/
* Glew: http://glew.sourceforge.net/
* Boost: https://www.boost.org/
* Websocketpp: https://www.zaphoyd.com/websocketpp
* Gdal: https://www.gdal.org/ [[Must be built with Geos]]  For Windows: http://www.gisinternals.com
* Geos: https://trac.osgeo.org/geos
* SDL2_image: https://www.libsdl.org/
* Intel TBB: https://www.threadingbuildingblocks.org/
* Visual C++ Redistributable for Visual Studio 2012 Update 4: https://www.microsoft.com/en-us/download/details.aspx?id=30679

#### Building

##### Emscripten

The web client:

``` Bash
$ git clone https://github.com/trailcode/webAsmPlay.git
$ cd webAsmPlay
$ mkdir buildEmscripten
$ cd buildEmscripten
$ emconfigure cmake ..
$ emmake make -j8
```

The native client:

``` Base
$ cd ..
$ mkdir buildNative
$ cd buildNative
$ cmake ..
$ make -j8
```

The server:

``` Base
cd ..
mkdir buildServer
cd buildServer
cmake ../GeoServer
make -j8
```

#### Running

Start the server:

``` Bash
cd ..
cd buildServer
./geoServer &
```

Run the web client:

``` Bash
cd ..
cd buildEmscripten
emrun --browser chrome index.html
```

// TODO The instructions are out of date!
You will need to move the the matrix panels out of the way and resize the
scene window panel. With the mouse in the scene window scrolling with the mouse
zooms in and out. Holding down the left shift key and moving the mouse rotates
the camera around the object. Holding down the left alt or option key pans the
camera on the XY plane.

Run the native client:

``` Bash
cd ..
cd buildNative
./webAsmPlay
```

### Notes

No notes.

