cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
#set(EMCC_DEBUG 1)
 
#set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD 14)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  message(" * C++ compiler: Emscripten")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
else()
  message(" * C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
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
                    "geos/src/linearref/*.cpp"
                    "geos/src/simplify/*.cpp")

file(GLOB GLUTesselatorSrc  "GLUTesselator/src/dict.cpp"
                            "GLUTesselator/src/mesh.cpp"
                            "GLUTesselator/src/render.cpp"
                            "GLUTesselator/src/tess.cpp"
                            "GLUTesselator/src/geom.cpp"
                            "GLUTesselator/src/memalloc.cpp"
                            "GLUTesselator/src/normal.cpp"
                            "GLUTesselator/src/priorityq.cpp"
                            "GLUTesselator/src/sweep.cpp"
                            "GLUTesselator/src/tessmono.cpp"
                            "GLUTesselator/src/tessellate.cpp" 
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

file(GLOB tceGeom_SRC       "./tceGeom/src/*.cpp")
file(GLOB tceOpenSteer_SRC  "./tceOpenSteer/src/*.cpp")
file(GLOB tceCore_SRC       "./tceCore/inc/tceCore/*.cpp")
file(GLOB tceRender_SRC     "./tceRender/src/*.cpp")

file(GLOB simpleJSON_SRC "SimpleJSON/src/*.cpp")

file(GLOB imguiSrc  "imgui/*.cpp")

file(GLOB webAsmPlay_SRC        "src/Camera.cpp"
                                "src/Debug.cpp"
                                "src/TrackBallInteractor.cpp"
                                "src/GUI.cpp"
                                "src/GeosTestPanel.cpp"
                                "src/RenderSettingsPanel.cpp"
                                "src/GUI_Settings_Panel.cpp"
                                "src/GUI_Persistence.cpp"
                                "src/InputEvents.cpp"
                                "src/Renderable.cpp"
                                "src/RenderableLineString.cpp"
                                "src/RenderablePolygon.cpp"
                                "src/RenderableCollection.cpp"
                                "src/RenderablePoint.cpp"
                                "src/GeosUtil.cpp"
                                "src/Util.cpp"
                                "src/FrameBuffer.cpp"
                                "src/Canvas.cpp"
                                "src/GeoClient.cpp"
                                "src/GeoClientRequest.cpp"
                                "src/GeosTestCanvas.cpp"
                                "src/SkyBox.cpp"
                                "src/Shader.cpp"
                                "src/Textures.cpp"
                                "src/GridPlane.cpp"
                                "src/AppLog.cpp"
                                "src/K2DTree.cpp"
                                "src/GeometryConverter.cpp"
                                "src/Attributes.cpp"
                                "src/Relation.cpp"
                                "src/ColorDistanceShader.cpp"
                                "src/main.cpp"
                                ${tceGeom_SRC}
                                ${tceCore_SRC}
                                ${tceOpenSteer_SRC}
                                ${tceRender_SRC})

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")

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
include_directories(GeoServer/inc)
include_directories(imgui)
include_directories(imguiAddons/imguitoolbar)
include_directories(SimpleJSON/inc)
include_directories(tceGeom/inc)
include_directories(tceCore/inc)
include_directories(tceOpenSteer/inc)
include_directories(tceRender/inc)
include_directories(inc)

add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)

set(sourcePath ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DSOURCE_BASE_PATH=R\"\(${sourcePath}\)\")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    
    SET(CMAKE_CXX_LINK_FLAGS   "--bind \
                                -s ALLOW_MEMORY_GROWTH=1 \
                                --no-heap-copy \
                                -s NO_EXIT_RUNTIME=1 \
                                -s WASM=1 \
                                -s USE_WEBGL2=1 \
                                -s USE_GLFW=3 \
                                --js-library ${CMAKE_CURRENT_SOURCE_DIR}/glfw-imgui-emscripten/library_glfw.js \
                                --js-library ${CMAKE_CURRENT_SOURCE_DIR}/glfw-imgui-emscripten/library_gl.js \
                                -s USE_SDL_IMAGE=2 \
                                -s SDL2_IMAGE_FORMATS='[\"png\"]' \
                                -s DISABLE_EXCEPTION_CATCHING=0 \
                                --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/ \
                                --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/config@/")

    add_definitions(-D__EMSCRIPTEN__)

    set(WEB_FILES
        index.html
        jquery.mousewheel.js
        styles/main.css
        )

    foreach(webfile ${WEB_FILES})
        ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${webfile}"
                COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/html/${webfile}"
                "${CMAKE_CURRENT_BINARY_DIR}/${webfile}"
                DEPENDS   "${CMAKE_CURRENT_SOURCE_DIR}/html/${webfile}"
                )
        list(APPEND webfile_dest "${CMAKE_CURRENT_BINARY_DIR}/${webfile}")

    endforeach(webfile)

    ADD_CUSTOM_TARGET(web ALL DEPENDS ${webfile_dest})

else()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

    find_package(glfw3 REQUIRED)
    find_package(Boost COMPONENTS filesystem REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    
    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${SDL2_INCLUDE_DIRS})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    include_directories(./ctpl)
    include_directories(./)
    
    link_libraries(${GLEW_LIBRARIES})
    link_libraries(glfw)
    link_libraries(${SDL2_IMAGE_LIBRARY})
    link_libraries(${SDL2_LIBRARIES})
    link_libraries(${Boost_LIBRARIES})

    set(ASSET_FILES
            back.png
            bot.png
            front.png
            left.png
            right.png
            top.png
            xneg.png
            xpos.png
            yneg.png
            ypos.png
            zneg.png
            zpos.png
            if_Info_131908.png
        )

    foreach(assetFile ${ASSET_FILES})
        ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}"
                COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}"
                "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}"
                DEPENDS   "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}"
                )
        list(APPEND assetFileDest "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}")

    endforeach(assetFile)

    ADD_CUSTOM_TARGET(assets ALL DEPENDS ${assetFileDest})

    set(config_files imgui.ini config.json)

    foreach(configFile ${config_files})
        ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${configFile}"
                COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}"
                "${CMAKE_CURRENT_BINARY_DIR}/${configFile}"
                DEPENDS   "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}"
                )
        list(APPEND configFileDest "${CMAKE_CURRENT_BINARY_DIR}/${configFile}")

    endforeach(assetFile)

    ADD_CUSTOM_TARGET(configs ALL DEPENDS ${configFileDest})

    if(APPLE)
	    SET(CMAKE_CXX_LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	endif()

endif()

file(GLOB sources   ${webAsmPlay_SRC}
                    ${geosSrc}
                    ${GLUTesselatorSrc}
                    ${simpleJSON_SRC}
                    )

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")

    add_executable(webAsmPlay ${sources} ${imguiSrc})

    add_executable(worker "src/Worker.cpp")

    set_target_properties(worker PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS=\"['_one']\" -s BUILD_AS_WORKER=1")

else()

    add_executable(webAsmPlay ${sources} ${imguiSrc} "imgui/gl3w/GL/gl3w.c")
endif()






