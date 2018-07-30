cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
 
set (CMAKE_CXX_STANDARD 11)
 
message(STATUS "Building with emcc")
    # which C and C++ compiler to use
    #SET(CMAKE_C_COMPILER   emcc)
    #SET(CMAKE_CXX_COMPILER em++)
    #SET(CMAKE_AR           emar)
    #SET(CMAKE_RANLIB       emranlib)
    #SET(CMAKE_C_FLAGS      -g)
    #SET(CMAKE_CXX_FLAGS    "-g -O2")
    #SET(CMAKE_CXX_FLAGS    "-O2")
#SET(CMAKE_CXX_FLAGS    "-O0")
    #SET(CMAKE_CXX_LINK_FLAGS "-s EXPORTED_FUNCTIONS=\"['_doIta', 'MyClass']\" --bind ")
#SET(CMAKE_CXX_LINK_FLAGS "-s --bind -s WASM=1")
#set(CMAKE_REQUIRED_FLAGS "${EMCC_LINKER_FLAGS}")

    # adjust the default behaviour of the FIND_XXX() commands:
    # search headers and libraries in the target environment, search
    # programs in the host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


    #SET( GMP_RUN_RES "0" CACHE STRING "Result from TRY_RUN" FORCE)
    #SET( GMP_RUN_RES__TRYRUN_OUTPUT "version=5.1.1" CACHE STRING "Output from TRY_RUN" FORCE)
    #SET( MPFR_RUN_RES "0" CACHE STRING "Result from TRY_RUN" FORCE)
    #SET( MPFR_RUN_RES__TRYRUN_OUTPUT "version=3.1.1" CACHE STRING "Output from TRY_RUN" FORCE)

    #set(EMCC_LINKER_FLAGS "-s SAFE_HEAP=1 --bind -s WASM=1")

    #add_definitions(-DEMCC_BUILD)
	#add_definitions(-DEMSCRIPTEN=1)
	#add_definitions(-DENVIRONMENT_IS_WEB=1)
	#add_definitions(-DENVIRONMENT_IS_NODE=0)
	#add_definitions("-s WASM=1")
	#add_definitions(-s NODERAWFS=0)
 
include_directories(geos/include)

#SET_SOURCE_FILES_PROPERTIES("test.cpp" PROPERTIES COMPILE_FLAGS -O0)

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

file(GLOB SOURCES "test.cpp")

add_executable(testStudent  ${SOURCES}
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



