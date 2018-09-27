cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
 
set (CMAKE_CXX_STANDARD 11)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  message(" * C++ compiler: Emscripten")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
else()
  message(" * C++ cimpiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

file(GLOB geosSrc   "geos/src/util/*.cpp"
                    "geos/src/geom/*.cpp"
                    "geos/src/index/bintree/*.cpp"
                    "geos/src/index/chain/*.cpp"
                    "geos/src/index/intervalrtree/*.cpp"
                    "geos/src/index/quadtree/*.cpp"
                    "geos/src/index/strtree/*.cpp"
                    "geos/src/index/sweepline/*.cpp"
                    "geos/src/geom/prep/*.cpp"
                    "geos/src/geom/util/*.cpp"
                    "geos/src/io/*.cpp"
                    "geos/src/algorithm/*.cpp"
                    "geos/src/algorithm/distance/*.cpp"
                    "geos/src/algorithm/locate/*.cpp"
                    "geos/src/geomgraph/*.cpp"
                    "geos/src/geomgraph/index/*.cpp"
                    "geos/src/noding/*.cpp"
                    "geos/src/noding/snapround/*.cpp"
                    "geos/src/operation/*.cpp"
                    "geos/src/operation/buffer/*.cpp"
                    "geos/src/operation/distance/*.cpp"
                    "geos/src/operation/intersection/*.cpp"
                    "geos/src/operation/linemerge/*.cpp"
                    "geos/src/operation/overlay/*.cpp"
                    "geos/src/operation/overlay/validate/*.cpp"
                    "geos/src/operation/overlay/snap/*.cpp"
                    "geos/src/operation/polygonize/*.cpp"
                    "geos/src/operation/predicate/*.cpp"
                    "geos/src/operation/relate/*.cpp"
                    "geos/src/operation/sharedpaths/*.cpp"
                    "geos/src/operation/union/*.cpp"
                    "geos/src/operation/valid/*.cpp"
                    "geos/src/planargraph/*.cpp"
                    "geos/src/planargraph/algorithm/*.cpp"
                    "geos/src/precision/*.cpp"
                    "geos/src/linearref/*.cpp")

file(GLOB GLUTesselatorSrc  "GLUTesselator/src/dict.cpp",
                            "GLUTesselator/src/mesh.cpp",
                            "GLUTesselator/src/render.cpp",
                            "GLUTesselator/src/tess.cpp",
                            "GLUTesselator/src/geom.cpp",
                            "GLUTesselator/src/memalloc.cpp",
                            "GLUTesselator/src/normal.cpp",
                            "GLUTesselator/src/priorityq.cpp",
                            "GLUTesselator/src/sweep.cpp",
                            "GLUTesselator/src/tessmono.cpp",
                            "GLUTesselator/src/tessellate.cpp"
                            )

file(GLOB tceGeom_SRC "./tceGeom/src/*.cpp" "./tceGeom/inc/tceGeom/*.h")
file(GLOB tceOpenSteer_SRC "./tceOpenSteer/src/*.cpp" "./tceOpenSteer/inc/tceOpenSteer/*.h")
file(GLOB tceCore_SRC "./tceCore/inc/tceCore/*.h")
file(GLOB tceRender_SRC "./tceRender/src/*.cpp" "./tceRender/inc/tceRender/*.h")

file(GLOB webAsmPlay_SRC   "test.cpp"
                            "Camera.cpp"
                            "Camera.h"
                            "debiug.cpp"
                            "debug.h"
                            "TrackBallInteractor.cpp"
                            "TrackBallInteractor.h"
                            "GUI.cpp"
                            ${tceGeom_SRC}
                            ${tceCore_SRC}
                            ${tceOpenSteer_SRC}
                            ${tceRender_SRC})

file(GLOB imguiEmscriptenSrc    "glfw-imgui-emscripten/glfw-imgui.cpp"
                                "glfw-imgui-emscripten/imgui_impl_glfw_gl3.cpp"
                                "glfw-imgui-emscripten/imgui_impl_glfw_gl3.h"
                                "glfw-imgui-emscripten/GLFW/glfw3.h"
                                "glfw-imgui-emscripten/imgui/imconfig.h"
                                "glfw-imgui-emscripten/imgui/imgui.cpp"
                                "glfw-imgui-emscripten/imgui/imgui.h"
                                "glfw-imgui-emscripten/imgui/imgui_demo.cpp"
                                "glfw-imgui-emscripten/imgui/imgui_draw.cpp"
                                "glfw-imgui-emscripten/imgui/imgui_internal.h"
                                "glfw-imgui-emscripten/imgui/imguidock.cpp"
                                "glfw-imgui-emscripten/imgui/imguidock.h"
                                "glfw-imgui-emscripten/imgui/stb_rect_pack.h"
                                "glfw-imgui-emscripten/imgui/stb_textedit.h"
                                "glfw-imgui-emscripten/imgui/stb_truetype.h"
                                )

file(GLOB imguiSrc              "imgui/imgui.cpp"
                                "imgui/imgui.h"
                                "imgui/imconfig.h"
                                "imgui/imgui_demo.cpp"
                                "imgui/imgui_draw.cpp"
                                "imgui/imgui_impl_glfw.cpp"
                                "imgui/imgui_impl_glfw.h"
                                "imgui/imgui_internal.h"
                                "imgui/imgui_widgets.cpp"
                                "imgui/imstb_rectpack.h"
                                "imgui/imstb_textedit.h"
                                "imgui/imstb_truetype.h"
                                "imgui/imgui_impl_opengl3.cpp"
                                "imgui/imgui_impl_opengl3.h"
                                "imgui/gl3w/GL/gl3w.c"                          
                                )

file(GLOB opensteerSrc  "opensteer/src/Clock.cpp"
                        "opensteer/src/Color.cpp"
                        "opensteer/src/Obstacle.cpp"
                        "opensteer/src/OldPathway.cpp"
                        "opensteer/src/Path.cpp"
                        "opensteer/src/Pathway.cpp"
                        "opensteer/src/PlugIn.cpp"
                        "opensteer/src/PolylineSegmentedPath.cpp"
                        "opensteer/src/PolylineSegmentedPathwaySegmentRadii.cpp"
                        "opensteer/src/PolylineSegmentedPathwaySingleRadius.cpp"
                        "opensteer/src/SegmentedPath.cpp"
                        "opensteer/src/SegmentedPathway.cpp"
                        "opensteer/src/SimpleVehicle.cpp"
                        "opensteer/src/TerrainRayTest.cpp"
                        "opensteer/src/TerrainRayTest.h"
                        "opensteer/src/Vec3.cpp"
                        "opensteer/src/Vec3Utilities.cpp"
                        "opensteer/src/lq.c"
                        "opensteer/src/Draw.cpp"
                        "opensteer/src/OpenSteerDemo.cpp"
                        "opensteer/plugins/Boids.cpp"
                        "opensteer/plugins/CaptureTheFlag.cpp"
                        "opensteer/plugins/LowSpeedTurn.cpp"
                        "opensteer/plugins/MapDrive.cpp"
                        "opensteer/plugins/MultiplePursuit.cpp"
                        "opensteer/plugins/OneTurning.cpp"
                        "opensteer/plugins/Pedestrian.cpp"
                        "opensteer/plugins/PedestriansWalkingAnEight.cpp"
                        "opensteer/plugins/Soccer.cpp"
                        )

file(GLOB globjectsSrc "globjects/source/*.cpp")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    include_directories(glfw-imgui-emscripten)
    include_directories(glfw-imgui-emscripten/imgui)
else()
    include_directories(imgui)
    include_directories(imgui/glfw/include)
    include_directories(imgui/gl3w)
endif()

include_directories(GLUTesselator/include)
include_directories(geos/include)
include_directories(glm)
include_directories(glbinding/include)
include_directories(glbinding-aux/include)
include_directories(globjects/include)
include_directories(opensteer/include)
include_directories(./tceGeom/inc)
include_directories(./tceCore/inc)
include_directories(./tceOpenSteer/inc)
include_directories(./tceRender/inc)
include_directories(./)

add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)
    
if(APPLE)
    SET(CMAKE_CXX_LINK_FLAGS "-framework OpenGL")
endif()

#add_library(imgui ${imguiSrc})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    message("Emscripten=========================")
    SET(CMAKE_CXX_LINK_FLAGS "-s USE_PTHREADS=1 -s FULL_ES2=1 -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap', 'setValue']\"")
    SET(CMAKE_CXX_LINK_FLAGS "-s NO_EXIT_RUNTIME=1 -s WASM=1 -s USE_WEBGL2=1 -s USE_GLFW=3 --js-library ../glfw-imgui-emscripten/library_glfw.js --js-library ../glfw-imgui-emscripten/library_gl.js ")
    add_definitions(-D__EMSCRIPTEN__)
else()

    find_package(glfw3 REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
    link_libraries(glfw)

endif()

#--------------------------------------------------------------------
# Detect and select backend APIs
#--------------------------------------------------------------------
if (GLFW_USE_WAYLAND)
    set(_GLFW_WAYLAND 1)
    message(STATUS "Using Wayland for window creation")
elseif (GLFW_USE_MIR)
    set(_GLFW_MIR 1)
    message(STATUS "Using Mir for window creation")
elseif (GLFW_USE_OSMESA)
    set(_GLFW_OSMESA 1)
    message(STATUS "Using OSMesa for headless context creation")
elseif (WIN32)
    set(_GLFW_WIN32 1)
    message(STATUS "Using Win32 for window creation")
elseif (APPLE)
    set(_GLFW_COCOA 1)
    message(STATUS "Using Cocoa for window creation")
elseif (UNIX)
    set(_GLFW_X11 1)
    message(STATUS "Using X11 for window creation")
else()
    message(FATAL_ERROR "No supported platform was detected")
endif()

#--------------------------------------------------------------------
# Add Vulkan static library if requested
#--------------------------------------------------------------------
if (GLFW_VULKAN_STATIC)
    if (VULKAN_FOUND AND VULKAN_STATIC_LIBRARY)
        list(APPEND glfw_LIBRARIES "${VULKAN_STATIC_LIBRARY}")
        if (BUILD_SHARED_LIBS)
            message(WARNING "Linking Vulkan loader static library into GLFW")
        endif()
    else()
        if (BUILD_SHARED_LIBS OR GLFW_BUILD_EXAMPLES OR GLFW_BUILD_TESTS)
            message(FATAL_ERROR "Vulkan loader static library not found")
        else()
            message(WARNING "Vulkan loader static library not found")
        endif()
    endif()
endif()

#--------------------------------------------------------------------
# Use Cocoa for window creation and NSOpenGL for context creation
#--------------------------------------------------------------------
if (_GLFW_COCOA)
    message("_GLFW_COCOA")
    list(APPEND glfw_LIBRARIES
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo")

    set(glfw_PKG_DEPS "")
    set(glfw_PKG_LIBS "-framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo")
endif()

if(APPLE)
    set_source_files_properties(glfw.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(ICON glfw.icns)
endif()

file(GLOB sources   ${webAsmPlay_SRC}
                    ${geosSrc}
                    ${GLUTesselatorSrc}
                    #${globjectsSrc}
                    ${opensteerSrc}
                    ${ICON}
                    )

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    add_executable(testStudent ${sources} ${imguiEmscriptenSrc} "mainWebAsm.cpp")
else()
    add_executable(testStudent ${sources} ${imguiSrc} "main.cpp")
endif()

if(APPLE)
    set_target_properties(testStudent PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "testStudent")

    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          RESOURCE glfw.icns
                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
                          MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION_FULL}
                          MACOSX_BUNDLE_ICON_FILE glfw.icns
                          MACOSX_BUNDLE_INFO_PLIST "${GLFW_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in")
endif()

set(WEB_FILES
        hi.html
        index.html
        triangle.js
        jquery.mousewheel.js
        styles/main.css
        )

message('fff', ${CMAKE_CURRENT_SOURCE_DIR})

foreach(webfile ${WEB_FILES})
    ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${webfile}"
            COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${webfile}"
            "${CMAKE_CURRENT_BINARY_DIR}/${webfile}"
            DEPENDS   "${CMAKE_CURRENT_SOURCE_DIR}/${webfile}"
            )
    list(APPEND webfile_dest "${CMAKE_CURRENT_BINARY_DIR}/${webfile}")

endforeach(webfile)

ADD_CUSTOM_TARGET(web ALL DEPENDS ${webfile_dest})


