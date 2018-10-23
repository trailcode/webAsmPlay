----------------------
WebAsmPlay
----------------------

WebAsmPlay was initially developed to evaluate web application development using Emscripten.

Currently a handful of C++ libraries have been compiled with Emscripten in this project to run in the browser
and run natively. The idea is to test a cross platform development environment where an application
is developed and debugged both as a web application and a browser application.
Another goal is to discover the limitations and capabilities of Emscipten web application development.
Cross platform development is achieved using cmake.

After initial evaluation the next goal of this project is to create a Real-TIme Strategy game using
OpenSteer (http://opensteer.sourceforge.net/)  and OpenStreetMap (https://www.openstreetmap.org)
To provide a starting point for non player character(NPC) zombie bots who roam Open Street Map cities.
NPC positions will be calculated on a server and the client application running in the browser or natively
can query bot positions and states for the given viewport. The server will also be responsible for geometry
generalization, bot AI, navigation graphs generated from Open Street Map data and state persistence. 

### Dependencies

Unfortunately during development notes on build depends were not tracked. Development is currently
being done on OSX. Macports (https://www.macports.org/) and Brew (https://brew.sh/) is being used to
install third party dependencies. C++ 17 is being used.

* Cmake: https://cmake.org
* Emscripten: http://kripken.github.io/emscripten-site
* Glfw3: https://www.glfw.org/
* Glew: http://glew.sourceforge.net/
* Boost: https://www.boost.org/
* Websocketpp: https://www.zaphoyd.com/websocketpp
* Gdal: https://www.gdal.org/
* Geos https://trac.osgeo.org/geos
* SDL2_image: https://www.libsdl.org/

### Sources

Some third party dependencies are built into the WebAsmPlay source tree to make emscripten compilation easer
and allow C++ 17 compilation:  
* Geos: https://trac.osgeo.org/geos
* OpenSteer: http://opensteer.sourceforge.net
* Ctpl: https://github.com/vit-vit/CTPL
* Glm: https://glm.g-truc.net/0.9.9/index.html
* GLUTessellator: https://github.com/mbebenita/GLUTesselator
* Native Dear Imgui: https://github.com/ocornut/imgui
* Imgui-addons: https://github.com/wflohry/imgui-addons
* Emscripten Dear Imgui: https://github.com/greggman/doodles/tree/master/glfw-imgui
* Websocketpp: https://github.com/zaphoyd/websocketpp

### Art and Data
* https://www.iconfinder.com/iconsets/32x32-free-design-icons

#### Building

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

You will need to move the the matrix panels out of the way and resize the
scene window panel. With the mouse in the scene window scrolling with the mouse
zoomes in and out. Holding down the left shift key and moving the mouse rotates
the camera around the object. Holding down the left alt or option key pans the
camera on the XY plane.

Run the native client:

``` Bash
cd ..
cd buildNative
./webAsmPlay
```

### Notes

  

