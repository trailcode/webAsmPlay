### Art and Data
* https://www.iconfinder.com/iconsets/32x32-free-design-icons


### Ideas

* Create geometry summary for the current viewport. Pie charts representing the area of land use, surface type, surface color, etc
  Also include roads, paths, and other linear features in this.

* Building to other surface ratios, roads, paths, grass, water, etc

* Try to classify the viewport to different attributes such as social economics, political distribution, city type, etc.
  Will require training.

* Split data into different layers. Possible layers could be transportation, linear feature infrastructure, rivers, etc. The objective
  is to separate linear features from features with area. The layers will be rendered to different offscreen buffers allowing for image
  processing.
  
    * ~~One filter to test would be setting the transparency depending on the distance from the camera.~~
    * Another filter would be to set the transparency based on the density of the features of similar color local to the current screen pixel.
      This also might be a start at generalization.
    * Transparency shader based on feature area or length.
      
* ~~Interpolate line string color in relation to the distance to the camera.~~

* Auto parse shader source to determine uniforms and attributes.
    * ~~Or better, pass into shader name and location reference pairs.~~

* Determine topological relationships of the polygonal features in the GeoServer.
  This will allow associating OSM building parts and extruding them on top of their parent geometry.

* Implement movable clip planes.

### TODO

* Make geometric terminology consistent with Geos.
* Fix dmess for GeoServer
* ~~Bring Emscripten ImGUI up to date with native version~~.
* Present error if cannot connect to server
* Move to a newer geos. 3.8.0Dev seems to have more functions.
* ~~Make a third party directory.~~
* Make a project Talent Stack Document/Panel.
* ~~Remove fill and outline color arguments in Renderable, RenderablePolygon, etc.~~
* Make tessellation templated.
* Generate OpenGL vertex buffers in the GeoServer.
* Assign default values of class member variables in header file. Must be a C++ 11 or C++ 14 feature.
* All renderables should use the VertexArrayObject class. This will be a step in using multiple GPU interfaces such as Vulkan, Direct3D, or Metal
* Zombie bots need to avoid walking through building walls.
* Make a unified camera class.
* Move OpenSteer Vec3 objects to glm::dvec objects.
* Detect OSM building polygons which have the same base but different heights. Use only the one with the smallest height.
    * Investigate this some more.
* Add overrides keywords. 
* Experiment with compute shaders for updating bot positions. 
* Add VLD and check for memory leaks.

### Bugs

* Holding down shift and moving outside the Geos test panel keeps it as if the shift key is still held down when the mouse is brought back in the panel.
* Camera still has a bug with window resizing and trackball. Seems to only happen in emscripten build.
* Upgraded OSX, glfw is having problems now :( Happened after upgrading to xcode 10
    * https://github.com/glfw/glfw/issues/1337
    * https://www.syntaxbomb.com/index.php?topic=4927.0
* Bots seem to get to the end of the path, but do not detect they are there and just spin around the end. 
* Geos Test Canvas is broken.
* Scrolling in Bing Maps framebuffer crashes.


## To Read
http://ogldev.atspace.co.uk/www/tutorial45/tutorial45.html

https://wiki.openstreetmap.org/wiki/OpenStreetCam

https://github.com/dkniffin/personal-website/blob/master/_drafts/2014-11-24-streetside-api.md

https://docs.microsoft.com/en-us/bingmaps/articles/bing-maps-tile-system

given bubble.key in base 10, convert to base 4 and get quadkey
load tiles: https://t.ssl.ak.tiles.virtualearth.net/tiles/hs0011023230012333103.jpg?g=6338&n=z
tiles indexed by quadkey using cube map: https://github.com/openstreetmap/iD/blob/76ffb5ef972b27b2a5658f0465282999da836b0f/modules/services/streetside.js#L866-L884

how to build quadkey: https://github.com/openstreetmap/iD/blob/76ffb5ef972b27b2a5658f0465282999da836b0f/modules/services/streetside.js#L866-L884

number of digits in quadkey depends on resolution: https://github.com/openstreetmap/iD/blob/76ffb5ef972b27b2a5658f0465282999da836b0f/modules/services/streetside.js#L372

https://github.com/openstreetmap/iD/blob/76ffb5ef972b27b2a5658f0465282999da836b0f/modules/services/streetside.js#L372