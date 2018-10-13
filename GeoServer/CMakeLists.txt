cmake_minimum_required(VERSION 2.8.9)
project(geoServer)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(gdal REQUIRED)
find_package(websocketpp REQUIRED)
find_package(geos REQUIRED)


include_directories(./inc)
include_directories(../inc)
include_directories(../ctpl)
include_directories(${GEOS_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GDAL_INCLUDE_DIR})
include_directories(${Websocketpp_INCLUDE_DIRS})


link_libraries(${Boost_LIBRARIES})
link_libraries(${GDAL_LIBRARY})
link_libraries(${GEOS_LIBRARY})

add_executable(geoServer src/GeoServer.cpp src/main.cpp)