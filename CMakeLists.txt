cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(default_build_type "Release")
set(CMAKE_BUILD_TYPE Release)

set(USE_FOLDERS ON)

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

set(EXPAT_INSTALL "/usr/local/Cellar/expat/2.2.6")

if(MSVC_VERSION)

	find_program(NUGET nuget)
	if(NOT NUGET)
	  message(FATAL "Cannot find nuget command line tool.\nInstall it with e.g. choco install nuget.commandline")
	else()

	  execute_process(COMMAND ${NUGET} install vii.SDL2_image			-Version 2.0.3		-o thirdParty)
	  execute_process(COMMAND ${NUGET} install boost_filesystem-vc141	-Version 1.69.0.0	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install boost_random-vc141		-Version 1.69.0.0	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install boost_date_time-vc141	-Version 1.69.0.0	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install boost_regex-vc141		-Version 1.69.0.0	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install curl						-Version 7.30.0.2	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install glfw						-Version 3.2.1.5	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install unofficial-flayan-glew	-Version 2.1.0		-o thirdParty)
	  execute_process(COMMAND ${NUGET} install tbb_oss					-Version 9.107.0	-o thirdParty)
	  execute_process(COMMAND ${NUGET} install expat					-Version 2.1.0.11	-o thirdParty)

	  file(GLOB boost_files ${webAsmPlay_BINARY_DIR}/thirdParty/boost_filesystem-vc141.1.69.0.0/lib/native/*
							${webAsmPlay_BINARY_DIR}/thirdParty/boost_random-vc141.1.69.0.0/lib/native/*
							${webAsmPlay_BINARY_DIR}/thirdParty/boost_date_time-vc141.1.69.0.0/lib/native/*
							${webAsmPlay_BINARY_DIR}/thirdParty/boost_regex-vc141.1.69.0.0/lib/native/*)

	  foreach(file ${boost_files})
		execute_process(COMMAND cmake -E copy ${file} ${webAsmPlay_BINARY_DIR}/thirdParty/boost.1.69.0.0/lib/)
	  endforeach(file)
	endif()

	set(TBB_LIB_PATH_SUFFIX /lib/x64/Debug /lib/x64/Release)
	
	set(BOOST_INCLUDEDIR				${webAsmPlay_BINARY_DIR}/thirdParty/boost.1.69.0.0/lib/native/include)
	set(BOOST_LIBRARYDIR				${webAsmPlay_BINARY_DIR}/thirdParty/boost.1.69.0.0/lib/)
	set(SDL2_PATH SDL2_SEARCH_PATHS		${webAsmPlay_BINARY_DIR}/thirdParty/sdl2.new.2.0.8/build/native)
	set(SDL2_IMAGE_PATH					${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.2.0.3/build/native)
	set(GLEW_LOCATION					${webAsmPlay_BINARY_DIR}/thirdParty/unofficial-flayan-glew.2.1.0/build/native)
	set(GLFW_ROOT_DIR					${webAsmPlay_BINARY_DIR}/thirdParty/glfw.3.2.1.5/build/native)
	set(TBB_ROOT_DIR					${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.9.107.0.0/build/native)
	set(EXPAT_INCLUDE_SEARCH_DIRS		${webAsmPlay_BINARY_DIR}/thirdParty/expat.2.1.0.11/build/native/include)
    set(EXPAT_LIB_SEARCH_DIRS			${webAsmPlay_BINARY_DIR}/thirdParty/expat.2.1.0.11/build/native/lib/v110/x64)
	set(CURL_INCLUDE_DIR				${webAsmPlay_BINARY_DIR}/thirdParty/curl.7.30.0.2/build/native/include)
	set(CURL_LIBRARY_RELEASE			${webAsmPlay_BINARY_DIR}/thirdParty/curl.7.30.0.2/build/native/lib/v110/x64/Release/dynamic/libcurl.lib)
	set(CURL_LIBRARY_DEBUG				${webAsmPlay_BINARY_DIR}/thirdParty/curl.7.30.0.2/build/native/lib/v110/x64/Debug/dynamic/libcurl.lib)

endif()

#set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD 14)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  message(" * C++ compiler: Emscripten")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
else()
  message(" * C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
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

file(GLOB simpleJSON_SRC            "ThirdParty/SimpleJSON/src/*.cpp")
file(GLOB imguiSrc                  "ThirdParty/imgui/*.cpp" "ThirdParty/imgui/*.h")
#file(GLOB gl3wSrc                   "ThirdParty/gl3w/src/gl3w.c" "ThirdParty/sl3w/inc/GL/*.h" "ThirdParty/sl3w/inc/KHR/*.h")
file(GLOB webAsmPlay_SRC            "inc/webAsmPlay/*.h" "src/*.cpp")
file(GLOB webAsmPlayShaders_SRC     "src/shaders/*.cpp" "inc/webAsmPlay/shaders/*.h")
file(GLOB webAsmPlayShadersGLSL_SRC "src/shaders/*.glsl")
file(GLOB webAsmPlayRenderables_SRC "src/renderables/*.cpp" "inc/webAsmPlay/renderables/*.h")
file(GLOB webAsmPlayGeom_SRC        "src/geom/*.cpp" "inc/webAsmPlay/geom/*.h")
file(GLOB webAsmPlayGUI_SRC	        "src/GUI/*.cpp""inc/webAsmPlay/GUI/*.h")
file(GLOB geoServer_SRC             "GeoServer/src/*.cpp" "GeoServer/inc/geoServer/*.h")
                  
file(GLOB sources   ${webAsmPlay_SRC}
					${webAsmPlayShaders_SRC}
					${webAsmPlayShadersGLSL_SRC}
                    ${webAsmPlayRenderables_SRC}
                    ${webAsmPlayGeom_SRC}
					${webAsmPlayGUI_SRC}
                    ${geosSrc}
                    ${GLUTesselatorSrc}
                    ${simpleJSON_SRC}
                    ${opensteerSrc}
					${imguiSrc}
					#${gl3wSrc}
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
	find_package(TBB REQUIRED)

	include_directories(${GLEW_INCLUDE_DIR})
    include_directories(${CURL_INCLUDE_DIR})
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
	include_directories(${GLFW3_INCLUDE_PATH})
	include_directories(${TBB_INCLUDE_DIRS})
    include_directories(ThirdParty/ctpl)
	#include_directories(ThirdParty/gl3w/inc)
    include_directories(./)
    
	link_libraries(debug ${CURL_LIBRARY_DEBUG} optimized ${CURL_LIBRARY_RELEASE})
	message(${GLFW3_LIBRARY})
	link_libraries(${GLFW3_LIBRARY})
    link_libraries(${SDL2_IMAGE_LIBRARY})
    link_libraries(${SDL2_LIBRARIES})
    link_libraries(${SDL2_LIBRARY})
	link_libraries(${OPENGL_gl_LIBRARY})
	link_libraries(${GLEW_LIBRARY})
    
    if(MSVC_VERSION)

        link_libraries(debug ${TBB_tbb_LIBRARY_DEBUG} optimized ${TBB_tbb_LIBRARY_RELEASE})

    else()

        link_libraries(${TBB_tbb_LIBRARY_RELEASE})

    endif()
    
	link_directories(${Boost_LIBRARY_DIRS})

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
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}"		COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}"			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${assetFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${assetFile}"	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${assetFile}"		COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${assetFile}"		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}"			COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}"			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")

			list(APPEND assetFileDestRelease		"${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}")
			list(APPEND assetFileDestRelWithDebInfo "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${assetFile}")
			list(APPEND assetFileDestMinSizeRel		"${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${assetFile}")
			list(APPEND assetFileDestDebug			"${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}")
			
		else()

			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
			list(APPEND assetFileDest "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}")
			
		endif()

    endforeach(assetFile)
	
    set(config_files imgui.ini config.json)

    foreach(configFile ${config_files})

		if(MSVC_VERSION)

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}"			COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}"		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}")
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}"	COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}")
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}"		COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}"		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}")
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}"			COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}"			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}")

			list(APPEND configFileDestRelease			"${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}")
			list(APPEND configFileDestRelWithDebInfo	"${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}")
			list(APPEND configFileDestMinSizeRel		"${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}")
			list(APPEND configFileDestDebug				"${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}")

		else()

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${configFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}" "${CMAKE_CURRENT_BINARY_DIR}/${configFile}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}")

			list(APPEND configFileDest "${CMAKE_CURRENT_BINARY_DIR}/${configFile}")

		endif()

    endforeach(configFile)

	foreach(shaderFilePath ${webAsmPlayShadersGLSL_SRC})

		get_filename_component(shaderFile ${shaderFilePath} NAME)

		if(MSVC_VERSION)

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${shaderFile}"			COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}"			MAIN_DEPENDENCY "${shaderFilePath}")
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${shaderFile}"	COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}"	MAIN_DEPENDENCY "${shaderFilePath}")
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${shaderFile}"		COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}"		MAIN_DEPENDENCY "${shaderFilePath}")
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${shaderFile}"			COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}"				MAIN_DEPENDENCY "${shaderFilePath}")

			list(APPEND shaderFileDestRelease			"${CMAKE_CURRENT_BINARY_DIR}/Release/${shaderFile}")
			list(APPEND shaderFileDestRelWithDebInfo	"${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${shaderFile}")
			list(APPEND shaderFileDestMinSizeRel		"${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${shaderFile}")
			list(APPEND shaderFileDestDebug				"${CMAKE_CURRENT_BINARY_DIR}/Debug/${shaderFile}")

		else()

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${shaderFile}" COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/${configFile}" DEPENDS "${shaderFilePath}")

			list(APPEND shaderFileDest "${CMAKE_CURRENT_BINARY_DIR}/${shaderFile}")

		endif()

    endforeach(shaderFile)

	if(MSVC_VERSION)

		ADD_CUSTOM_TARGET(assetsRelease			ALL DEPENDS ${assetFileDestRelease})
		ADD_CUSTOM_TARGET(assetsRelWithDebInfo	ALL DEPENDS ${assetFileDestRelWithDebInfo})
		ADD_CUSTOM_TARGET(assetsMinSizeRel		ALL DEPENDS ${assetFileDestMinSizeRel})
		ADD_CUSTOM_TARGET(assetsDebug			ALL DEPENDS ${assetFileDestDebug})
		ADD_CUSTOM_TARGET(configsRelease		ALL DEPENDS ${configFileDestRelease})
		ADD_CUSTOM_TARGET(configsRelWithDebInfo ALL DEPENDS ${configFileDestRelWithDebInfo})
		ADD_CUSTOM_TARGET(configsMinSizeRel		ALL DEPENDS ${configFileDestMinSizeRel})
		ADD_CUSTOM_TARGET(configsDebug			ALL DEPENDS ${configFileDestDebug})
		ADD_CUSTOM_TARGET(shadersRelease		ALL DEPENDS ${shaderFileDestRelease})
		ADD_CUSTOM_TARGET(shadersRelWithDebInfo	ALL DEPENDS ${shaderFileDestRelWithDebInfo})
		ADD_CUSTOM_TARGET(shadersMinSizeRel		ALL DEPENDS ${shaderFileDestMinSizeRel})
		ADD_CUSTOM_TARGET(shadersDebug			ALL DEPENDS ${shaderFileDestDebug})

	else()

		ADD_CUSTOM_TARGET(configs ALL DEPENDS ${configFileDest})
		ADD_CUSTOM_TARGET(assets ALL DEPENDS ${assetFileDest})
		ADD_CUSTOM_TARGET(shaders ALL DEPENDS ${shaderFileDest})

	endif()

    if(APPLE)
	    SET(CMAKE_CXX_LINK_FLAGS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
	endif()

	#add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
	#add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)
	add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")

    add_executable(webAsmPlay ${sources} "src/Main.cpp")
    
    add_executable(worker "src/Worker.cpp")

    set_target_properties(worker PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS=\"['_one']\" -s BUILD_AS_WORKER=1")

else()

	add_library(libWebAsmPlay STATIC ${sources})
	
	add_executable(webAsmPlay "src/Main.cpp")
	target_link_libraries(webAsmPlay libWebAsmPlay)

	find_package(Expat REQUIRED)
	find_package(Boost COMPONENTS filesystem system REQUIRED)
	
	add_executable(geoServer ${geoServer_SRC})
    
	target_include_directories(geoServer PRIVATE "ThirdParty/LAStools")
	target_include_directories(geoServer PRIVATE ${EXPAT_INCLUDE_DIRS})
	target_include_directories(geoServer PRIVATE ${Boost_INCLUDE_DIRS})

	target_link_libraries(geoServer libWebAsmPlay)
    target_link_libraries(geoServer ${EXPAT_LIBRARIES}
                            debug ${Boost_FILESYSTEM_LIBRARY_DEBUG} optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE}
                            debug ${Boost_SYSTEM_LIBRARY_DEBUG} optimized ${Boost_SYSTEM_LIBRARY_RELEASE})

	if(MSVC_VERSION)

	    message("Compiler is MSVC")

		# Tell MSVC to execute in the build directory of the current configuration.
		set_property(TARGET webAsmPlay PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest /MP")

        #install(FILES	${BOOST_FILESYSTEM_PATH_RELEASE}/${BOOST_FILESYSTEM_BASE_NAME_RELEASE2}.dll
                        #${BOOST_SYSTEM_PATH}/${BOOST_SYSTEM_BASE_NAME2}.dll
                        #"${SDL2_IMAGE_LIBRARY_PATH}/SDL2_image.dll"
                        #"${SDL2MAIN_LIBRARY_PATH}/SDL2.dll"
        #DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release
        #CONFIGURATIONS Release 
        #RelWithDebInfo
        #)

		set(DLLS_COMMON		${webAsmPlay_BINARY_DIR}/thirdParty/unofficial-flayan-glew.redist.2.1.0/build/native/bin/x64/dynamic/glew32.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/sdl2.new.redist.2.0.8/build/native/bin/x64/SDL2.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libjpeg-9.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libpng16-16.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libtiff-5.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libwebp-7.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/SDL2_image.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/zlib1.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/glfw.3.2.1.5/build/native/bin/dynamic/v140/x64/glfw3.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/zlib.v140.windesktop.msvcstl.dyn.rt-dyn.1.2.8.8/lib/native/v140/windesktop/msvcstl/dyn/rt-dyn/x64/MinSizeRel/zlib.dll)

        set(DLLS_RELEASE	
							${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.redist.9.107.0.0/build/native/bin/x64/Release/tbb.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/expat.redist.2.1.0.11/build/native/bin/v110/x64/Release/dynamic/utf8/libexpat.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/curl.redist.7.30.0.2/build/native/bin/v110/x64/Release/dynamic/libcurl.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/libssh2.redist.1.4.3.1/build/native/bin/v110/x64/Release/dynamic/cdecl/libssh2.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/openssl.redist.1.0.1.25/build/native/bin/v120/x64/Release/dynamic/libeay32.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/openssl.redist.1.0.1.25/build/native/bin/v120/x64/Release/dynamic/ssleay32.dll)

		set(DLLS_DEBUG	
							${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.redist.9.107.0.0/build/native/bin/x64/Debug/tbb_debug.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/expat.redist.2.1.0.11/build/native/bin/v110/x64/Debug/dynamic/utf8/libexpat.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/curl.redist.7.30.0.2/build/native/bin/v110/x64/Debug/dynamic/libcurl.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/libssh2.redist.1.4.3.1/build/native/bin/v110/x64/Debug/dynamic/cdecl/libssh2.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/openssl.redist.1.0.1.25/build/native/bin/v120/x64/Debug/dynamic/libeay32.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/openssl.redist.1.0.1.25/build/native/bin/v120/x64/Debug/dynamic/ssleay32.dll)

        foreach(dll ${DLLS_RELEASE} ${DLLS_COMMON})
            add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/Release")
			add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo")
			add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel")
        endforeach(dll)
		
        foreach(dll ${DLLS_DEBUG} ${DLLS_COMMON})
            add_custom_command(TARGET webAsmPlay POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${dll}" "${CMAKE_CURRENT_BINARY_DIR}/Debug")
        endforeach(dll)


        #get_filename_component(UTF_BASE_NAME_DEBUG ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG} NAME_WE)
        #install(FILES ${UTF_PATH}/${UTF_BASE_NAME_DEBUG}.dll
        #  DESTINATION ../bin
        #  CONFIGURATIONS Debug
        #)
	endif()

endif()

source_group("OpenSteer"					FILES ${opensteerSrc})
source_group("imgui"						FILES ${imguiSrc})
source_group("simpleJSON"					FILES ${simpleJSON_SRC})
source_group("GLUTesselator"				FILES ${GLUTesselatorSrc})
source_group("Geos"							FILES ${geosSrc})
source_group("webAsmPlay"					FILES ${webAsmPlay_SRC})
source_group("webAsmPlay\\shaders"			FILES ${webAsmPlayShaders_SRC})
source_group("webAsmPlay\\shaders\\glsl"	FILES ${webAsmPlayShadersGLSL_SRC})
source_group("webAsmPlay\\renderables"		FILES ${webAsmPlayRenderables_SRC})
source_group("webAsmPlay\\geom"				FILES ${webAsmPlayGeom_SRC})
source_group("webAsmPlay\\GUI"				FILES ${webAsmPlayGUI_SRC})







