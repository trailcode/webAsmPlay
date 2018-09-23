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

file(GLOB utilSrc "geos/src/util/*.cpp")
file(GLOB geomSrc "geos/src/geom/*.cpp")
file(GLOB indexBinTreeSrc "geos/src/index/bintree/*.cpp")
file(GLOB indexChainSrc "geos/src/index/chain/*.cpp")
file(GLOB indexIntervalrtreeSrc "geos/src/index/intervalrtree/*.cpp")
file(GLOB indexQuadtreeSrc "geos/src/index/quadtree/*.cpp")
file(GLOB indexStrtreeSrc "geos/src/index/strtree/*.cpp")
file(GLOB indexSweeplineSrc "geos/src/index/sweepline/*.cpp")
file(GLOB geomPrepSrc "geos/src/geom/prep/*.cpp")
file(GLOB geomUtilSrc "geos/src/geom/util/*.cpp")
file(GLOB iOSrc "geos/src/io/*.cpp")
file(GLOB algorithmSrc "geos/src/algorithm/*.cpp")
file(GLOB algorithmDistanceSrc "geos/src/algorithm/distance/*.cpp")
file(GLOB algorithmLocateSrc "geos/src/algorithm/locate/*.cpp")
file(GLOB geomgraphSrc "geos/src/geomgraph/*.cpp")
file(GLOB geomgraphIndexSrc "geos/src/geomgraph/index/*.cpp")
file(GLOB nodingSrc "geos/src/noding/*.cpp")
file(GLOB nodingSnaproundSrc "geos/src/noding/snapround/*.cpp")
file(GLOB operationSrc "geos/src/operation/*.cpp")
file(GLOB operationBufferSrc "geos/src/operation/buffer/*.cpp")
file(GLOB operationDistanceSrc "geos/src/operation/distance/*.cpp")
file(GLOB operationIntersectionSrc "geos/src/operation/intersection/*.cpp")
file(GLOB operationLinemergeSrc "geos/src/operation/linemerge/*.cpp")
file(GLOB operationOverlaySrc "geos/src/operation/overlay/*.cpp")
file(GLOB operationOverlayValidateSrc "geos/src/operation/overlay/validate/*.cpp")
file(GLOB operationOverlaySnapSrc "geos/src/operation/overlay/snap/*.cpp")
file(GLOB operationPolygonizeSrc "geos/src/operation/polygonize/*.cpp")
file(GLOB operationPredicateSrc "geos/src/operation/predicate/*.cpp")
file(GLOB operationRelateSrc "geos/src/operation/relate/*.cpp")
file(GLOB operationSharedpathsSrc "geos/src/operation/sharedpaths/*.cpp")
file(GLOB operationUnionSrc "geos/src/operation/union/*.cpp")
file(GLOB operationValidSrc "geos/src/operation/valid/*.cpp")
file(GLOB planargraphSrc "geos/src/planargraph/*.cpp")
file(GLOB planargraphAlgorithmSrc "geos/src/planargraph/algorithm/*.cpp")
file(GLOB precisionSrc "geos/src/precision/*.cpp")
file(GLOB linearrefSrc "geos/src/linearref/*.cpp")

include_directories(GLUTesselator/include)

file(GLOB GLUTesselatorSrc "GLUTesselator/src/dict.cpp",
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

file(GLOB SOURCES   "test.cpp"
                    "Camera.cpp"
                    "Camera.h"
                    "debiug.cpp"
                    "debug.h"
                    "TrackBallInteractor.cpp"
                    "TrackBallInteractor.h"
                    ${tceGeom_SRC}
                    ${tceCore_SRC}
                    ${tceOpenSteer_SRC}
                    ${tceRender_SRC})

file(GLOB imguiSrc  "glfw-imgui/glfw-imgui.cpp"
                    "glfw-imgui/imgui_impl_glfw_gl3.cpp"
                    "glfw-imgui/imgui_impl_glfw_gl3.h"
                    "glfw-imgui/GLFW/glfw3.h"
                    "glfw-imgui/imgui/imconfig.h"
                    "glfw-imgui/imgui/imgui.cpp"
                    "glfw-imgui/imgui/imgui.h"
                    "glfw-imgui/imgui/imgui_demo.cpp"
                    "glfw-imgui/imgui/imgui_draw.cpp"
                    "glfw-imgui/imgui/imgui_internal.h"
                    "glfw-imgui/imgui/stb_rect_pack.h"
                    "glfw-imgui/imgui/stb_textedit.h"
                    "glfw-imgui/imgui/stb_truetype.h"
                    )

include_directories(geos/include)
include_directories(glfw-imgui)
include_directories(glfw-imgui/imgui)
include_directories(glm)
INCLUDE_DIRECTORIES(./tceGeom/inc)
INCLUDE_DIRECTORIES(./tceCore/inc)
INCLUDE_DIRECTORIES(./tceOpenSteer/inc)
INCLUDE_DIRECTORIES(./tceRender/inc)
INCLUDE_DIRECTORIES(./)

add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)
    
if(APPLE)
    SET(CMAKE_CXX_LINK_FLAGS "-framework OpenGL")
endif()

#add_library(imgui ${imguiSrc})

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    SET(CMAKE_CXX_LINK_FLAGS "-s USE_PTHREADS=1 -s FULL_ES2=1 -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap', 'setValue']\"")
    SET(CMAKE_CXX_LINK_FLAGS "-s NO_EXIT_RUNTIME=1 -s WASM=1 -s USE_WEBGL2=1 -s USE_GLFW=3 --js-library ../glfw-imgui/library_glfw.js --js-library ../glfw-imgui/library_gl.js ")
else()

    find_package(glfw3 REQUIRED)
    find_package(glew REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
    link_libraries(${OPENGL_gl_LIBRARY} glfw)
    message("Glew" ${GLEW_LIBRARIES})

endif()

add_executable(testStudent  ${SOURCES}
                            ${imguiSrc}
                            ${geomSrc}
                            ${geomPrepSrc}
                            ${geomUtilSrc}
                            ${iOSrc}
                            ${algorithmSrc}
                            ${algorithmDistanceSrc}
                            ${algorithmLocateSrc}
                            ${utilSrc}
                            ${indexBinTreeSrc}
                            ${indexChainSrc}
                            ${indexIntervalrtreeSrc}
                            ${indexQuadtreeSrc}
                            ${indexStrtreeSrc}
                            ${indexSweeplineSrc}
                            ${geomgraphSrc}
                            ${geomgraphIndexSrc}
                            ${nodingSrc}
                            ${nodingSnaproundSrc}
                            ${operationSrc}
                            ${operationBufferSrc}
                            ${operationDistanceSrc}
                            ${operationIntersectionSrc}
                            ${operationLinemergeSrc}
                            ${operationOverlaySrc}
                            ${operationOverlayValidateSrc}
                            ${operationOverlaySnapSrc}
                            ${operationPolygonizeSrc}
                            ${operationPredicateSrc}
                            ${operationRelateSrc}
                            ${operationSharedpathsSrc}
                            ${operationUnionSrc}
                            ${operationValidSrc}
                            ${planargraphSrc}
                            ${planargraphAlgorithmSrc}
                            ${precisionSrc}
                            ${linearrefSrc}
                            ${GLUTesselatorSrc}
)

#target_link_libraries(testStudent geos imgui)

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


