cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(default_build_type "Release")
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
#set(EMCC_DEBUG 1)

set(USE_FOLDERS ON)

if(MSVC_VERSION)

    set(BOOST_ROOT "C:/tp/boost_1_69_0")
    set(SDL2_PATH "C:/tp/SDL-2.0.9")
    set(SDL2_IMAGE_PATH "C:/tp/SDL2_image-2.0.4")
    set(GLEW_LOCATION "C:/tp/glew-2.1.0")
    set(GLFW_ROOT_DIR "C:/tp/glfw-3.2.1")
	set(TBB_ROOT_DIR "C:/tp/tbb2019_20190206oss")
    #set(GEOS_ROOT "C:/tp/geos-3.7.0")
    #set(GEOS_LIBRARY "C:/tp/geos_Release_3_5_0_x64/lib/geos_i.lib")
    #set(GEOS_INCLUDE_DIR "C:/tp/geos_Release_3_5_0_x64/include")
    set(PC_EXPAT_INCLUDE_DIRS "C:/tp/expat-2.2.6/include")
    set(PC_EXPAT_LIBRARY_DIRS "C:/tp/expat-2.2.6/lib")
    set(GDAL_LIBRARY "C:/tp/gdal_Release_2_3_2_x64/lib/gdal_i.lib")
    set(GDAL_INCLUDE_DIR "C:/tp/gdal_Release_2_3_2_x64/include")
	set(CURL_INCLUDE_DIR "C:/tp/curl-7.64.0/include")
	set(CURL_LIBRARY "C:/tp/curl-7.64.0/lib/libcurl_imp.lib")

endif()

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

file(GLOB LAStoolSrc "ThirdParty/LAStools/*.cpp")

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
                        "ThirdParty/opensteer/src/lq2D.cpp"
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
                        "ThirdParty/opensteer/plugins/Soccer.cpp")

file(GLOB simpleJSON_SRC "ThirdParty/SimpleJSON/src/*.cpp")

file(GLOB imguiSrc  "ThirdParty/imgui/*.cpp" "ThirdParty/imgui/*.h")

file(GLOB gl3wSrc  "ThirdParty/gl3w/src/gl3w.c" "ThirdParty/sl3w/inc/GL/*.h" "ThirdParty/sl3w/inc/KHR/*.h")

file(GLOB webAsmPlay_SRC    "inc/webAsmPlay/*.h"
                            "src/*.cpp")

file(GLOB webAsmPlayShaders_SRC "src/shaders/*.cpp"
								"inc/webAsmPlay/shaders/*.h")

file(GLOB webAsmPlayRenderables_SRC "src/renderables/*.cpp"
									"inc/webAsmPlay/renderables/*.h")

file(GLOB webAsmPlayGUI_SRC			"src/GUI/*.cpp"
									"inc/webAsmPlay/GUI/*.h")

file(GLOB geoServer_SRC     "GeoServer/src/*.cpp"
                            "GeoServer/inc/geoServer/*.h"
                            #${LAStoolSrc}
                            #"src/Network.cpp"
                            )
                  
file(GLOB sources   ${webAsmPlay_SRC}
					${webAsmPlayShaders_SRC}
					${webAsmPlayRenderables_SRC}
					${webAsmPlayGUI_SRC}
                    ${geosSrc}
                    ${GLUTesselatorSrc}
                    ${simpleJSON_SRC}
                    ${opensteerSrc}
					${imguiSrc}
					${gl3wSrc}
                    #"ThirdParty/imgui/glad/glad.c"
                    )
				  
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

#add_definitions(-DIMGUI_API)

set(sourcePath ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DSOURCE_BASE_PATH=R\"\(${sourcePath}\)\")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    
    #--js-library ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-imgui-emscripten/library_glfw.js \
    #--js-library ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-imgui-emscripten/library_gl.js \
    #--js-library /Users/trailcode/emsdk/emscripten/1.37.35/src/library_gl.js \

    SET(CMAKE_CXX_LINK_FLAGS   "--bind \
                                -s ALLOW_MEMORY_GROWTH=1 \
                                --no-heap-copy \
                                -s NO_EXIT_RUNTIME=1 \
                                -s WASM=1 \
                                -s USE_WEBGL2=1 \
                                -s USE_GLFW=3 \
                                --js-library ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-imgui-emscripten/library_glfw.js \
                                --js-library ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw-imgui-emscripten/library_gl.js \
                                -s USE_SDL=2 \
                                -s USE_SDL_IMAGE=2 \
                                -s SDL2_IMAGE_FORMATS='[\"png\"]' \
                                -s DISABLE_EXCEPTION_CATCHING=0 \
                                -s FETCH=1 \
                                -s DEMANGLE_SUPPORT=1 \
                                --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/ \
                                --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/config@/")

    #-s DEMANGLE_SUPPORT=1 \
    #-s ASSERTIONS=2 \
    #set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-s DEMANGLE_SUPPORT=1")

    add_definitions(-D__EMSCRIPTEN__)

    set(WEB_FILES   index.html
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

	find_package(OpenGL)
    find_package(glfw3 REQUIRED)
    find_package(Boost COMPONENTS filesystem random REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
	find_package(GLEW REQUIRED)
    find_package(curl REQUIRED)
	find_package(TBB REQUIRED)

	#add_definitions(-DGLEW_STATIC)

	include_directories(${GLEW_INCLUDE_DIR})
    include_directories(${CURL_INCLUDE_DIR})
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
	include_directories(${GLFW3_INCLUDE_PATH})
	include_directories(${TBB_INCLUDE_DIRS})
    include_directories(ThirdParty/ctpl)
	include_directories(ThirdParty/gl3w/inc)
    include_directories(./)
    
    #link_libraries(${GLEW_LIBRARIES})
    link_libraries(${CURL_LIBRARIES})
	message(${GLFW3_LIBRARY})
	link_libraries(${GLFW3_LIBRARY})
    link_libraries(${SDL2_IMAGE_LIBRARY})
    link_libraries(${SDL2_LIBRARIES})
    link_libraries(${SDL2_LIBRARY})
	link_libraries(debug ${TBB_tbb_LIBRARY_DEBUG} optimized ${TBB_tbb_LIBRARY_RELEASE})
	#set(Boost_USE_STATIC_LIBS ON)
    link_libraries(${Boost_LIBRARIES})
	link_libraries(${OPENGL_gl_LIBRARY})
	link_libraries(${GLEW_LIBRARY})
	
	link_directories(${Boost_LIBRARY_DIRS})

	message("=================== " ${CMAKE_CURRENT_BINARY_DIR})

    set(ASSET_FILES     back.png
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
		if(MSVC_VERSION)
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")

			list(APPEND assetFileDestRelease "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}")
			list(APPEND assetFileDestDebug "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}")
			
		else()
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
			list(APPEND assetFileDest "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}")
			
		endif()
    endforeach(assetFile)
	
	if(MSVC_VERSION)
		ADD_CUSTOM_TARGET(assetsRelease ALL DEPENDS ${assetFileDestRelease})
		ADD_CUSTOM_TARGET(assetsDebug ALL DEPENDS ${assetFileDestDebug})
	else()
		ADD_CUSTOM_TARGET(assets ALL DEPENDS ${assetFileDest})
	endif()

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

	#add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
	#add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)
	add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")

    add_executable(webAsmPlay ${sources} "src/Main.cpp")
    #target_link_libraries(webAsmPlay libWebAsmPlay)

    add_executable(worker "src/Worker.cpp")

    set_target_properties(worker PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS=\"['_one']\" -s BUILD_AS_WORKER=1")

else()

    #set(CMAKE_CXX_FLAGS "-fsanitize=address -O1 -fno-omit-frame-pointer -g")

    #add_executable(webAsmPlay ${sources} "ThirdParty/imgui/gl3w/GL/gl3w.c")
	add_library(libWebAsmPlay STATIC ${sources})
	#add_executable(webAsmPlay ${webAsmPlay_SRC} ${opensteerSrc} ${imguiSrc} "ThirdParty/imgui/gl3w/GL/gl3w.c")

	add_executable(webAsmPlay "src/Main.cpp")
	target_link_libraries(webAsmPlay libWebAsmPlay)

	find_package(expat REQUIRED)
	find_package(Boost COMPONENTS filesystem REQUIRED)
	#find_package(gdal REQUIRED)

	#project(geoServer)

    #add_executable(geoServer ${geoServer_SRC} "ThirdParty/imgui/glad/glad.c")
	add_executable(geoServer ${geoServer_SRC})
    target_include_directories(geoServer PRIVATE "ThirdParty/LAStools")
	target_include_directories(geoServer PRIVATE ${EXPAT_INCLUDE_DIRS})
	target_include_directories(geoServer PRIVATE ${Boost_INCLUDE_DIRS})
	#target_include_directories(geoServer PRIVATE ${GDAL_INCLUDE_DIR})
	target_link_libraries(geoServer libWebAsmPlay)
	#target_link_libraries(geoServer ${GDAL_LIBRARY})
	target_link_libraries(geoServer ${Boost_LIBRARIES})
	target_link_libraries(geoServer ${EXPAT_LIBRARIES})

	#add_subdirectory(GeoServer)

	if(MSVC_VERSION)

	    message("Compiler is MSVC")

		# Tell MSVC to execute in the build directory of the current configuration.
		set_property(TARGET webAsmPlay PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest /MP")

        get_filename_component(BOOST_FILESYSTEM_BASE_NAME_RELEASE ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE)
        get_filename_component(BOOST_FILESYSTEM_BASE_NAME_DEBUG ${Boost_FILESYSTEM_LIBRARY_DEBUG} NAME_WE)
        
        get_filename_component(BOOST_FILESYSTEM_PATH_RELEASE ${Boost_FILESYSTEM_LIBRARY_RELEASE} PATH)
        get_filename_component(BOOST_FILESYSTEM_PATH_DEBUG ${Boost_FILESYSTEM_LIBRARY_DEBUG} PATH)
        
        string(SUBSTRING ${BOOST_FILESYSTEM_BASE_NAME_RELEASE} 3 -1 BOOST_FILESYSTEM_BASE_NAME_RELEASE2) # Strip lib chars at beginning
        string(SUBSTRING ${BOOST_FILESYSTEM_BASE_NAME_DEBUG} 3 -1 BOOST_FILESYSTEM_BASE_NAME_DEBUG2) # Strip lib chars at beginning
        
        get_filename_component(SDL2_IMAGE_LIBRARY_PATH ${SDL2_IMAGE_LIBRARY} DIRECTORY)
        get_filename_component(SDL2MAIN_LIBRARY_PATH ${SDL2MAIN_LIBRARY} DIRECTORY)

        get_filename_component(CURL_LIBRARIES_PATH ${CURL_LIBRARIES} DIRECTORY)

		message("==================================== " ${BOOST_FILESYSTEM_BASE_NAME_RELEASE})

        get_filename_component(EXPAT_LIBRARY_PATH ${EXPAT_LIBRARY} DIRECTORY)

        #install(FILES	${BOOST_FILESYSTEM_PATH_RELEASE}/${BOOST_FILESYSTEM_BASE_NAME_RELEASE2}.dll
                        #${BOOST_SYSTEM_PATH}/${BOOST_SYSTEM_BASE_NAME2}.dll
                        #"${SDL2_IMAGE_LIBRARY_PATH}/SDL2_image.dll"
                        #"${SDL2MAIN_LIBRARY_PATH}/SDL2.dll"
        #DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release
        #CONFIGURATIONS Release 
        #RelWithDebInfo
        #)

        set(DLLS_RELEASE	
							#"${BOOST_FILESYSTEM_PATH_RELEASE}/${BOOST_FILESYSTEM_BASE_NAME_RELEASE2}.dll"
							"${BOOST_FILESYSTEM_PATH_RELEASE}/${BOOST_FILESYSTEM_BASE_NAME_RELEASE}.dll"
                            "${SDL2_IMAGE_LIBRARY_PATH}/SDL2_image.dll"
                            "${SDL2MAIN_LIBRARY_PATH}/../bin/SDL2.dll"
                            "${CURL_LIBRARIES_PATH}/../bin/libcurl.dll"
                            "${EXPAT_LIBRARY_PATH}/../bin/expat.dll")

        set(DLLS_DEBUG		#"${BOOST_FILESYSTEM_PATH_DEBUG}/${BOOST_FILESYSTEM_BASE_NAME_DEBUG2}.dll"
							"${BOOST_FILESYSTEM_PATH_DEBUG}/${BOOST_FILESYSTEM_BASE_NAME_DEBUG}.dll"
                            "${SDL2_IMAGE_LIBRARY_PATH}/SDL2_image.dll"
                            "${SDL2MAIN_LIBRARY_PATH}/../bin/SDL2.dll"
                            "${CURL_LIBRARIES_PATH}/../bin/libcurl.dll"
                            "${EXPAT_LIBRARY_PATH}/../bin/expat.dll")

        foreach(dll ${DLLS_RELEASE})
            add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/Release")
        endforeach(dll)

		foreach(dll ${DLLS_RELEASE})
            add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo")
        endforeach(dll)

        foreach(dll ${DLLS_DEBUG})
            add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/Debug")
        endforeach(dll)

        #get_filename_component(UTF_BASE_NAME_DEBUG ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG} NAME_WE)
        #install(FILES ${UTF_PATH}/${UTF_BASE_NAME_DEBUG}.dll
        #  DESTINATION ../bin
        #  CONFIGURATIONS Debug
        #)
	endif()

endif()

source_group("OpenSteer"				FILES ${opensteerSrc})
source_group("imgui"					FILES ${imguiSrc})
source_group("simpleJSON"				FILES ${simpleJSON_SRC})
source_group("GLUTesselator"			FILES ${GLUTesselatorSrc})
source_group("Geos"						FILES ${geosSrc})
source_group("webAsmPlay"				FILES ${webAsmPlay_SRC})
source_group("webAsmPlay\\shaders"      FILES ${webAsmPlayShaders_SRC})
source_group("webAsmPlay\\renderables"  FILES ${webAsmPlayRenderables_SRC})
source_group("webAsmPlay\\GUI"			FILES ${webAsmPlayGUI_SRC})







