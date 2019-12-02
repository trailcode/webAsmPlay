----------------------
WebAsmPlay
----------------------

WebAsmPlay was initially developed to evaluate web application development using Emscripten.
Currently the Emscripten part is not working. Hopefully in the future it will be. 

[![Alt text](https://img.youtube.com/vi/s0unMIQUs1U/0.jpg)](https://www.youtube.com/watch?v=s0unMIQUs1U)<br/>
YouTube video of current native development version.

To see the last Emscripten build working: https://trailcode.github.io/ZombiGeoSim/index.html Be patient to let it load.
Hold down the "alt" key and move the mouse to pan. Hold down the "shift" key and move the mouse to rotate the camera.

The current goal of this project is morphing into a Real-Time Strategy game using
OpenSteer (http://opensteer.sourceforge.net/) and OpenStreetMap (https://www.openstreetmap.org)
 
### Features

* Ability to run the client in a supported web browser or as a native client. 
    * Object Picking where the picked object under the mouse cursor is highlighted and it's attributes are displayed.
    * Skybox Render.
    * Path finding over linear features.
    * Autonomous zombies wonder on paths while steering to avoid each other.
    * Multiple camera modes: TrackBall and zombie tracking camera mode.
    * User definable symbology color rendering supporting transparency for both fill and outlines.
        * Camera near and far symbology can be defined for each attribute allowing for interpolated symbology blending proportional to object to camera distance.
    * Ability to render 3D buildings.
    * Projection of BingMaps raster tiles over scene geometry. 
    * Create and play camera animation paths.

* Geometry Server
    * ESRI Shapefile and OpenStreetMap XML GIS formats with linestring and polygon ingestion along with attribution.
    * Fast custom attributed transport protocol.
    * Ensures linear feature topology correctness. Breaks linestrings at intersections and connections. Removes overlapping linestrings.
    * Discover polygon topographical relations. Child(contained), parent, and neighbors. 

### Sources

Some third party dependencies are built into the WebAsmPlay source tree to make emscripten compilation easer
and allow C++ 17 compilation:  
* Geos: https://trac.osgeo.org/geos
* OpenSteer: http://opensteer.sourceforge.net
* Ctpl: https://github.com/vit-vit/CTPL
* Glm: https://glm.g-truc.net/0.9.9/index.html
* GLUTessellator: https://github.com/mbebenita/GLUTesselator
* Native Dear Imgui: https://github.com/ocornut/imgui [Docking branch]
* Imgui-addons: https://github.com/wflohry/imgui-addons
* Emscripten Dear Imgui: https://github.com/greggman/doodles/tree/master/glfw-imgui
* Websocketpp: https://github.com/zaphoyd/websocketpp
* Histogram: https://github.com/HDembinski/histogram
* SimpleJSON: https://github.com/MJPA/SimpleJSON

#### Building

The build system is based on CMake. 

### OSX

* brew install boost
* brew install glfw3
    * brew link glfw
* brew install expat keg-only does not link in /usr/local
* brew install assimp
* brew install sdl2_image

#### Bugs

* ~~dmessError does not flush output~~

#### Todo

* Replace curlUtil::BufferStruct with a std::pair
* Remove the model component from the canvas, simplify the shaders.
* Light pos for PhongShader, PhongShaderInstanced, ColorDistanceDepthShader3D and ColorDistanceShader is inconsistent. 
* Unify textures. Bing map tiles and bubble tiles. Create mechanism to prune tile textures from GPU.
* Models need to work with SSAO pipeline.
* Add ctpl thread pool to CMakeLists.txt so it is editable.
* Create dedicated threadpools for file IO.
* Search for thread_pool and ensure correct implementation for stack or queue depending on usage
  * Consider priority queue system for bing tiles and street side tiles
  * Ensure thread pools are shutting down currectly.
* Create a performance watchdog to break if a frame is taking too long to render.


#### Notes

* https://docs.microsoft.com/en-us/bingmaps/articles/getting-streetside-tiles-from-imagery-metadata



