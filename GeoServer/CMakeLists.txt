cmake_minimum_required(VERSION 2.8.9)
project(geoServer)

set (CMAKE_CXX_STANDARD 11)

include_directories(./inc)
include_directories(../inc)

add_executable(geoServer src/GeoServer.cpp src/main.cpp)