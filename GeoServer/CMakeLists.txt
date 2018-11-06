cmake_minimum_required(VERSION 2.8.9)
project(geoServer)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(gdal REQUIRED)
find_package(websocketpp REQUIRED)
find_package(geos REQUIRED)
find_package(expat REQUIRED)

include_directories(${GEOS_INCLUDE_DIR})
include_directories(./inc)
include_directories(../inc)
include_directories(../ThirdParty/ctpl)
include_directories(../ThirdParty/histogram/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GDAL_INCLUDE_DIR})
include_directories(${EXPAT_INCLUDE_DIRS})
include_directories(${Websocketpp_INCLUDE_DIRS})

link_libraries(${Boost_LIBRARIES})
link_libraries(${GDAL_LIBRARY})
link_libraries(${GEOS_LIBRARIES})
link_libraries(${EXPAT_LIBRARIES})
link_libraries(/usr/local/Cellar/geos/3.7.0/lib/libgeos-3.7.0.dylib)

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
                            src/main.cpp)