cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(default_build_type "Release")
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

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
      # Default to -O3 when no build type is specified.
      # append_compiler_flag("-O3")
endif()

file(GLOB geosSrc   "ThirdParty/geos/src/util/*.cpp"
                    "ThirdParty/geos/src/geom/*.cpp"
                    "ThirdParty/geos/src/index/bintree/*.cpp"
                    "ThirdParty/geos/src/index/chain/*.cpp"
                    "ThirdParty/geos/src/index/intervalrtree/*.cpp"
                    "ThirdParty/geos/src/index/quadtree/*.cpp"
                    "ThirdParty/geos/src/index/strtree/*.cpp"
                    "ThirdParty/geos/src/index/sweepline/*.cpp"
                    "ThirdParty/geos/src/geom/prep/*.cpp"
                    "ThirdParty/geos/src/geom/util/*.cpp"
                    "ThirdParty/geos/src/io/*.cpp"
                    "ThirdParty/geos/src/algorithm/*.cpp"
                    "ThirdParty/geos/src/algorithm/distance/*.cpp"
                    "ThirdParty/geos/src/algorithm/locate/*.cpp"
                    "ThirdParty/geos/src/geomgraph/*.cpp"
                    "ThirdParty/geos/src/geomgraph/index/*.cpp"
                    "ThirdParty/geos/src/noding/*.cpp"
                    "ThirdParty/geos/src/noding/snapround/*.cpp"
                    "ThirdParty/geos/src/operation/*.cpp"
                    "ThirdParty/geos/src/operation/buffer/*.cpp"
                    "ThirdParty/geos/src/operation/distance/*.cpp"
                    "ThirdParty/geos/src/operation/intersection/*.cpp"
                    "ThirdParty/geos/src/operation/linemerge/*.cpp"
                    "ThirdParty/geos/src/operation/overlay/*.cpp"
                    "ThirdParty/geos/src/operation/overlay/validate/*.cpp"
                    "ThirdParty/geos/src/operation/overlay/snap/*.cpp"
                    "ThirdParty/geos/src/operation/polygonize/*.cpp"
                    "ThirdParty/geos/src/operation/predicate/*.cpp"
                    "ThirdParty/geos/src/operation/relate/*.cpp"
                    "ThirdParty/geos/src/operation/sharedpaths/*.cpp"
                    "ThirdParty/geos/src/operation/union/*.cpp"
                    "ThirdParty/geos/src/operation/valid/*.cpp"
                    "ThirdParty/geos/src/planargraph/*.cpp"
                    "ThirdParty/geos/src/planargraph/algorithm/*.cpp"
                    "ThirdParty/geos/src/precision/*.cpp"
                    "ThirdParty/geos/src/linearref/*.cpp"
                    "ThirdParty/geos/src/simplify/*.cpp")

file(GLOB GLUTesselatorSrc  "ThirdParty/GLUTesselator/src/*.cpp")

file(GLOB opensteerSrc  "ThirdParty/opensteer/src/Clock.cpp"
                            "ThirdParty/opensteer/src/Color.cpp"
                            "ThirdParty/opensteer/src/Obstacle.cpp"
                            "ThirdParty/opensteer/src/OldPathway.cpp"
                            "ThirdParty/opensteer/src/Path.cpp"
                            "ThirdParty/opensteer/src/Pathway.cpp"
                            "ThirdParty/opensteer/src/PlugIn.cpp"
                            "ThirdParty/opensteer/src/PolylineSegmentedPath.cpp"
                            "ThirdParty/opensteer/src/PolylineSegmentedPathwaySegmentRadii.cpp"
                            "ThirdParty/opensteer/src/PolylineSegmentedPathwaySingleRadius.cpp"
                            "ThirdParty/opensteer/src/SegmentedPath.cpp"
                            "ThirdParty/opensteer/src/SegmentedPathway.cpp"
                            "ThirdParty/opensteer/src/SimpleVehicle.cpp"
                            "ThirdParty/opensteer/src/TerrainRayTest.cpp"
                            "ThirdParty/opensteer/src/TerrainRayTest.h"
                            "ThirdParty/opensteer/src/Vec3.cpp"
                            "ThirdParty/opensteer/src/Vec3Utilities.cpp"
                            "ThirdParty/opensteer/src/lq.c"
                            "ThirdParty/opensteer/src/Draw.cpp"
                            "ThirdParty/opensteer/src/OpenSteerDemo.cpp"
                            "ThirdParty/opensteer/src/Camera.cpp"
                            "ThirdParty/opensteer/plugins/Boids.cpp"
                            "ThirdParty/opensteer/plugins/CaptureTheFlag.cpp"
                            "ThirdParty/opensteer/plugins/LowSpeedTurn.cpp"
                            "ThirdParty/opensteer/plugins/MapDrive.cpp"
                            "ThirdParty/opensteer/plugins/MultiplePursuit.cpp"
                            "ThirdParty/opensteer/plugins/OneTurning.cpp"
                            "ThirdParty/opensteer/plugins/Pedestrian.cpp"
                            "ThirdParty/opensteer/plugins/PedestriansWalkingAnEight.cpp"
                            "ThirdParty/opensteer/plugins/Soccer.cpp"
                            )

file(GLOB tceGeom_SRC       "./tceGeom/src/*.cpp")
file(GLOB tceOpenSteer_SRC  "./tceOpenSteer/src/*.cpp")
file(GLOB tceCore_SRC       "./tceCore/inc/tceCore/*.cpp")
file(GLOB tceRender_SRC     "./tceRender/src/*.cpp")

file(GLOB simpleJSON_SRC "ThirdParty/SimpleJSON/src/*.cpp")

file(GLOB imguiSrc  "ThirdParty/imgui/*.cpp")

file(GLOB webAsmPlay_SRC        "src/GUI/GUI.cpp"
                                "src/GUI/GeosTestPanel.cpp"
                                "src/GUI/RenderSettingsPanel.cpp"
                                "src/GUI/GUI_Settings_Panel.cpp"
                                "src/GUI/GUI_Persistence.cpp"
                                "src/GUI/SymbologyPanel.cpp"
                                "src/GUI/InputEvents.cpp"
                                "src/GUI/OpenSteerTestPanel.cpp"
                                "src/shaders/Shader.cpp"
                                "src/shaders/ShaderProgram.cpp"
                                "src/shaders/ColorShader.cpp"
                                "src/shaders/ColorDistanceShader.cpp"
                                "src/shaders/ColorDistanceShader2.cpp"
                                "src/shaders/ColorVertexShader.cpp"
                                "src/Camera.cpp"
                                "src/Debug.cpp"
                                "src/TrackBallInteractor.cpp"
                                "src/Renderable.cpp"
                                "src/RenderableLineString.cpp"
                                "src/RenderablePolygon.cpp"
                                "src/RenderableCollection.cpp"
                                "src/RenderablePoint.cpp"
                                "src/RenderableMesh.cpp"
                                "src/DeferredRenderable.cpp"
                                "src/GeosUtil.cpp"
                                "src/Util.cpp"
                                "src/FrameBuffer.cpp"
                                "src/Canvas.cpp"
                                "src/GeoClient.cpp"
                                "src/GeoClientRequest.cpp"
                                "src/GeosTestCanvas.cpp"
                                "src/SkyBox.cpp"
                                "src/Textures.cpp"
                                "src/GridPlane.cpp"
                                "src/K2DTree.cpp"
                                "src/GeometryConverter.cpp"
                                "src/Attributes.cpp"
                                "src/Relation.cpp"
                                "src/OpenSteerGlue.cpp"
                                "src/OpenSteerCanvas.cpp"
                                "src/main.cpp"
                                ${tceGeom_SRC}
                                ${tceCore_SRC}
                                ${tceOpenSteer_SRC}
                                ${tceRender_SRC})

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")

    include_directories(ThirdParty/imgui/glfw/include)
    include_directories(ThirdParty/imgui/gl3w)

endif()

include_directories(ThirdParty)
include_directories(ThirdParty/GLUTesselator/include)
include_directories(ThirdParty/geos/include)
include_directories(ThirdParty/glm)
include_directories(ThirdParty/imgui)
include_directories(ThirdParty/imguiAddons/imguitoolbar)
include_directories(ThirdParty/opensteer/include)
include_directories(ThirdParty/SimpleJSON/inc)
include_directories(GeoServer/inc)
include_directories(tceGeom/inc)
include_directories(tceCore/inc)
include_directories(tceOpenSteer/inc)
include_directories(tceRender/inc)
include_directories(inc)

add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_RADIANS)

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
                                --js-library ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-imgui-emscripten/library_glfw.js \
                                --js-library ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-imgui-emscripten/library_gl.js \
                                -s USE_SDL_IMAGE=2 \
                                -s SDL2_IMAGE_FORMATS='[\"png\"]' \
                                -s DISABLE_EXCEPTION_CATCHING=0 \
                                -s FETCH=1 \
                                --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/ \
                                --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/config@/")

    #-s DEMANGLE_SUPPORT=1 \
    #set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-s DEMANGLE_SUPPORT=1")

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
    include_directories(ThirdParty/ctpl)
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

    endforeach(configFile)

    ADD_CUSTOM_TARGET(configs ALL DEPENDS ${configFileDest})

    if(APPLE)
	    SET(CMAKE_CXX_LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	endif()

endif()

file(GLOB sources   ${webAsmPlay_SRC}
                    ${geosSrc}
                    ${GLUTesselatorSrc}
                    ${simpleJSON_SRC}
                    ${opensteerSrc}
                    )

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")

    add_executable(webAsmPlay ${sources} ${imguiSrc})

    add_executable(worker "src/Worker.cpp")

    set_target_properties(worker PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS=\"['_one']\" -s BUILD_AS_WORKER=1")

else()

    #set(CMAKE_CXX_FLAGS "-fsanitize=address -O1 -fno-omit-frame-pointer -g")

    add_executable(webAsmPlay ${sources} ${imguiSrc} "ThirdParty/imgui/gl3w/GL/gl3w.c")
endif()






