cmake_minimum_required(VERSION 2.8.9)
project(geoServer)

set(BOOST_ROOT "C:/boost_1_68_0")
set(SDL2_PATH "C:/tp/SDL2-2.0.9")
set(SDL2_IMAGE_PATH "C:/tp/SDL2_image-2.0.4")
set(GLEW_LOCATION "C:/tp/glew-2.1.0")
set(GLFW_ROOT_DIR "C:/tp/glfw-3.2.1")
set(GEOS_ROOT "C:/tp/geos-3.7.0")
#set(GEOS_LIBRARY "C:/tp/geos_Release_3_5_0_x64/lib/geos_i.lib")
#set(GEOS_INCLUDE_DIR "C:/tp/geos_Release_3_5_0_x64/include")
set(PC_EXPAT_INCLUDE_DIRS "C:/tp/expat_Release_2_1_0_x64/include")
set(PC_EXPAT_LIBRARY_DIRS "C:/tp/expat_Release_2_1_0_x64/lib")
set(GDAL_LIBRARY "C:/tp/gdal_Release_2_3_2_x64/lib/gdal_i.lib")
set(GDAL_INCLUDE_DIR "C:/tp/gdal_Release_2_3_2_x64/include")

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(gdal REQUIRED)
#find_package(websocketpp REQUIRED)
find_package(geos REQUIRED)
find_package(expat REQUIRED)
find_package(glfw3 REQUIRED)
#find_package(tbb REQUIRED)

include_directories(${GEOS_INCLUDE_DIR})
include_directories(./inc)
include_directories(../inc)
include_directories(../ThirdParty/ctpl)
include_directories(../ThirdParty/histogram/include)
include_directories(../ThirdParty/opensteer/include)
include_directories(../ThirdParty/imgui) # TODO try to remove
include_directories(../ThirdParty/websocketpp)
include_directories(../ThirdParty/glm)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GDAL_INCLUDE_DIR})
include_directories(${EXPAT_INCLUDE_DIRS})
include_directories(${GLFW3_INCLUDE_PATH})
#include_directories(${Websocketpp_INCLUDE_DIRS})

link_libraries(${Boost_LIBRARIES})
link_libraries(${GDAL_LIBRARY})
#link_libraries(${GEOS_LIBRARIES})
link_libraries(${GEOS_LIBRARY})
link_libraries(${EXPAT_LIBRARIES})
#link_libraries(/usr/local/Cellar/geos/3.7.0/lib/libgeos-3.7.0.dylib)

message("${GDAL_LIBRARY}")

set(sourcePath "${CMAKE_CURRENT_SOURCE_DIR}../")

add_definitions(-DSOURCE_BASE_PATH=R\"\(${sourcePath}\)\")
add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)
add_definitions(-DXML_LARGE_SIZE)

add_executable(geoServer    ../src/GeometryConverter.cpp
                            ../src/Attributes.cpp
                            ../src/Relation.cpp
                            ../src/GeosUtil.cpp
                            src/DataModelOSM.cpp
                            src/OSM_Reader.cpp
                            src/GeoServer.cpp
                            src/TopologyLineStrings.cpp
                            src/TopologyPolygons.cpp
                            src/AttributeStatistics.cpp
                            src/main.cpp)