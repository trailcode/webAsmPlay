cmake_minimum_required(VERSION 2.8.9)
project(webAsmPlay)
set(default_build_type "Release")
#set(CMAKE_BUILD_TYPE Debug)

set(USE_FOLDERS ON)

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

if(MSVC_VERSION)

    #set(BOOST_VERSION "1.69.0.0")
	set(BOOST_VERSION   "1.71.0.0")
    set(GLFW_VERSION    "3.3.0.1")
    set(GLEW_VERSION    "2.1.0")
    set(EXPAT_VERSION   "2.2.9")
	set(VC_VERSION      "142")

	set(CURL_VC_VERSION "141") # Still only version 141?

    set(Boost_COMPILER -vc${VC_VERSION})

	find_program(NUGET nuget)
	if(NOT NUGET)
	  message(FATAL "Cannot find nuget command line tool.\nInstall it with e.g. choco install nuget.commandline")
	else()

		execute_process(COMMAND ${NUGET} install vii.SDL2_image				-Version 2.0.3		-o thirdParty)
		
        #execute_process(COMMAND ${NUGET} install boost_filesystem-vc${VC_VERSION}		-Version ${BOOST_VERSION}	-o thirdParty)
		#execute_process(COMMAND ${NUGET} install boost_random-vc${VC_VERSION}			-Version ${BOOST_VERSION}	-o thirdParty)
		#execute_process(COMMAND ${NUGET} install boost_date_time-vc${VC_VERSION}		-Version ${BOOST_VERSION}	-o thirdParty)
		#execute_process(COMMAND ${NUGET} install boost_regex-vc${VC_VERSION}			-Version ${BOOST_VERSION}	-o thirdParty)
        #execute_process(COMMAND ${NUGET} install boost_system-vc${VC_VERSION}			-Version ${BOOST_VERSION}	-o thirdParty)
        #execute_process(COMMAND ${NUGET} install boost_python37-vc${VC_VERSION}			-Version ${BOOST_VERSION}	-o thirdParty)
		#execute_process(COMMAND ${NUGET} install boost_numpy37-vc${VC_VERSION}			-Version ${BOOST_VERSION}	-o thirdParty)

		execute_process(COMMAND ${NUGET} install curl-vc${CURL_VC_VERSION}-dynamic-x86_64	-Version 7.59.0		-o thirdParty)
		execute_process(COMMAND ${NUGET} install glfw						-Version ${GLFW_VERSION}	-o thirdParty)
		execute_process(COMMAND ${NUGET} install unofficial-flayan-glew		-Version ${GLEW_VERSION}		-o thirdParty)
		execute_process(COMMAND ${NUGET} install tbb_oss					-Version 9.107.0.0	-o thirdParty)
		execute_process(COMMAND ${NUGET} install expat.v${VC_VERSION}		-Version ${EXPAT_VERSION}		-o thirdParty)
		execute_process(COMMAND ${NUGET} install assimp.v140				-Version 3.2.0		-o thirdParty)
	
		#file(GLOB boost_files	${webAsmPlay_BINARY_DIR}/thirdParty/boost_filesystem-vc${VC_VERSION}.${BOOST_VERSION}/lib/native/*
		#						${webAsmPlay_BINARY_DIR}/thirdParty/boost_system-vc${VC_VERSION}.${BOOST_VERSION}/lib/native/*
		#						${webAsmPlay_BINARY_DIR}/thirdParty/boost_random-vc${VC_VERSION}.${BOOST_VERSION}/lib/native/*
		#						${webAsmPlay_BINARY_DIR}/thirdParty/boost_date_time-vc${VC_VERSION}.${BOOST_VERSION}/lib/native/*
		#						${webAsmPlay_BINARY_DIR}/thirdParty/boost_regex-vc${VC_VERSION}.${BOOST_VERSION}/lib/native/*
		#						${webAsmPlay_BINARY_DIR}/thirdParty/boost_python37-vc${VC_VERSION}.${BOOST_VERSION}/lib/native/*)
		#
		#foreach(file ${boost_files})
		#	execute_process(COMMAND cmake -E copy ${file} ${webAsmPlay_BINARY_DIR}/thirdParty/boost.${BOOST_VERSION}/lib/)
		#endforeach(file)
	endif()


	set(TBB_LIB_PATH_SUFFIX /lib/x64/Debug /lib/x64/Release)

	set(BOOST_INCLUDEDIR				C:/boost_1_72_0)
    #set(BOOST_LIBRARYDIR				C:/boost_1_72_0/lib64-msvc-14.2)
    set(BOOST_LIBRARYDIR				C:/boost_1_72_0/stage/lib)

	#set(BOOST_INCLUDEDIR				${webAsmPlay_BINARY_DIR}/thirdParty/boost.${BOOST_VERSION}/lib/native/include)
	#set(BOOST_LIBRARYDIR				${webAsmPlay_BINARY_DIR}/thirdParty/boost.${BOOST_VERSION}/lib/)
	set(SDL2_PATH SDL2_SEARCH_PATHS		${webAsmPlay_BINARY_DIR}/thirdParty/sdl2.new.2.0.8/build/native)
	set(SDL2_IMAGE_PATH					${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.2.0.3/build/native)
	set(GLEW_LOCATION					${webAsmPlay_BINARY_DIR}/thirdParty/unofficial-flayan-glew.${GLEW_VERSION}/build/native)
	set(GLFW_ROOT_DIR					${webAsmPlay_BINARY_DIR}/thirdParty/glfw.${GLFW_VERSION}/build/native)
	set(TBB_ROOT_DIR					${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.9.107.0.0/build/native)
	set(TBB_tbb_debug_LIBRARY			${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.9.107.0.0/build/native/lib/x64/Debug/)
	set(TBB_tbb_LIBRARY					${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.9.107.0.0/build/native/lib/x64/Release/)
	set(EXPAT_INCLUDE_SEARCH_DIRS		${webAsmPlay_BINARY_DIR}/thirdParty/expat.v${VC_VERSION}.${EXPAT_VERSION}/build/native/include)
    set(EXPAT_LIB_SEARCH_DIRS			${webAsmPlay_BINARY_DIR}/thirdParty/expat.v${VC_VERSION}.${EXPAT_VERSION}/build/native/lib/x64/Debug/
										${webAsmPlay_BINARY_DIR}/thirdParty/expat.v${VC_VERSION}.${EXPAT_VERSION}/build/native/lib/x64/Release/)
	set(CURL_INCLUDE_DIR				${webAsmPlay_BINARY_DIR}/thirdParty/curl-vc${CURL_VC_VERSION}-dynamic-x86_64.7.59.0/build/native/include)
	set(CURL_LIBRARY_RELEASE			${webAsmPlay_BINARY_DIR}/thirdParty/curl-vc${CURL_VC_VERSION}-dynamic-x86_64.7.59.0/build/native/lib/x64/static/libcurl.lib)
	set(CURL_LIBRARY_DEBUG				${webAsmPlay_BINARY_DIR}/thirdParty/curl-vc${CURL_VC_VERSION}-dynamic-x86_64.7.59.0/build/native/lib/x64/static/libcurl_debug.lib)
	set(ASSIMP_ROOT_DIR					${webAsmPlay_BINARY_DIR}/thirdParty/assimp.v140.3.2/build/native)
	set(ASSIMP_INCLUDE_DIR				${webAsmPlay_BINARY_DIR}/thirdParty/assimp.v140.3.2/build/native/include)
	set(ASSIMP_LIBRARY_DEBUG			${webAsmPlay_BINARY_DIR}/thirdParty/assimp.v140.3.2/build/native/lib/x64/Debug/assimp-vc140-mtd.lib)
	set(ASSIMP_LIBRARY_RELEASE			${webAsmPlay_BINARY_DIR}/thirdParty/assimp.v140.3.2/build/native/lib/x64/Release/assimp-vc140-mt.lib)
	
else()
    set(CURL_LIBRARY_RELEASE ${CURL_LIBRARY})
    set(CURL_LIBRARY_DEBUG   ${CURL_LIBRARY})
endif()

set (CMAKE_CXX_STANDARD 17)
#set (CMAKE_CXX_STANDARD 14)

if (EMSCRIPTEN)
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
file(GLOB imguiSrc                  "ThirdParty/imgui/*.h"	"ThirdParty/imgui/*.cpp" )
file(GLOB webAsmPlay_SRC            "inc/webAsmPlay/*.h"	"src/*.cpp")
file(GLOB webAsmPlayShaders_SRC     "src/shaders/*.cpp"		"inc/webAsmPlay/shaders/*.h")
file(GLOB webAsmPlayShadersGLSL_SRC "src/shaders/*.glsl")
file(GLOB webAsmPlayRenderables_SRC "src/renderables/*.cpp" "inc/webAsmPlay/renderables/*.h")
file(GLOB webAsmPlayGeom_SRC        "src/geom/*.cpp"		"inc/webAsmPlay/geom/*.h")
file(GLOB webAsmPlayGUI_SRC	        "src/GUI/*.cpp"			"inc/webAsmPlay/GUI/*.h")
file(GLOB webAsmPlayCanvas_SRC      "src/canvas/*.cpp"		"inc/webAsmPLay/canvas/*.h")
file(GLOB webAsmPlayBing_SRC		"src/bing/*.cpp"		"inc/webAsmPlay/bing/*.h")
file(GLOB geoServer_SRC             "GeoServer/src/*.cpp"	"GeoServer/inc/geoServer/*.h")

file(GLOB webAsmPlayPython_SRC "py/*.py")

set(ASSET_FILES     ${CMAKE_CURRENT_SOURCE_DIR}/assets/back.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/bot.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/front.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/left.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/right.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/top.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/xneg.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/xpos.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/yneg.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/ypos.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/zneg.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/zpos.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/if_Info_131908.png
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/tree1.obj
                    ${CMAKE_CURRENT_SOURCE_DIR}/assets/tree1.mtl
					${CMAKE_CURRENT_SOURCE_DIR}/assets/ProggyClean.ttf)

set(config_files	${CMAKE_CURRENT_SOURCE_DIR}/config/imgui.ini
					${CMAKE_CURRENT_SOURCE_DIR}/config/config.json)
                  
file(GLOB sources   ${webAsmPlay_SRC}
					${webAsmPlayShaders_SRC}
					${webAsmPlayShadersGLSL_SRC}
                    ${webAsmPlayRenderables_SRC}
                    ${webAsmPlayGeom_SRC}
					${webAsmPlayGUI_SRC}
					${webAsmPlayCanvas_SRC}
					${webAsmPlayBing_SRC}
					${webAsmPlayPython_SRC}
                    ${geosSrc}
                    ${GLUTesselatorSrc}
                    ${simpleJSON_SRC}
                    ${opensteerSrc}
					${imguiSrc}
					${ASSET_FILES}
					${config_files}
					readme.md
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
include_directories(ThirdParty/nlohmann.json/inc)
include_directories(GeoServer/inc)
include_directories(inc)

set(sourcePath ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DSOURCE_BASE_PATH=R\"\(${sourcePath}\)\")
add_definitions(-DUSE_UNSTABLE_GEOS_CPP_API)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#add_definitions(-DBOOST_PYTHON_STATIC_LIB)
#add_definitions(-DBOOST_NUMPY_STATIC_LIB)
add_definitions(-DBOOST_LIB_DIAGNOSTIC)
#add_definitions(-DBOOST_ALL_NO_LIB)

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
								-s USE_BOOST_HEADERS=1 \
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

	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-s USE_BOOST_HEADERS=1 -s USE_WEBGL2=1")

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

	add_executable(webAsmPlay ${sources} "src/Main.cpp")
    
    add_executable(worker "src/Worker.cpp")

    set_target_properties(worker PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS=\"['_one']\" -s BUILD_AS_WORKER=1")


else()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

    #find_package(Boost COMPONENTS filesystem random python38 REQUIRED)
    find_package(Boost COMPONENTS filesystem python37 REQUIRED)
	find_package(OpenGL			REQUIRED)
    find_package(glfw3			REQUIRED)
    find_package(SDL2			REQUIRED)
    find_package(SDL2_image		REQUIRED)
	find_package(GLEW			REQUIRED)
    find_package(TBB			REQUIRED)
    find_package(curl			REQUIRED)
    find_package(Expat			REQUIRED)
    find_package(Assimp         REQUIRED)
    find_package(PythonLibs 3.7 EXACT REQUIRED)
	#find_package(LIBIGL			REQUIRED)
    
    message(GLFW3_INCLUDE_PATH ${GLFW3_INCLUDE_PATH})
    
	include_directories(${GLEW_INCLUDE_DIR})
    include_directories(${CURL_INCLUDE_DIR})
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
	include_directories(${GLFW3_INCLUDE_PATH})
	include_directories(${TBB_INCLUDE_DIRS})
    include_directories(${ASSIMP_INCLUDE_DIR})
    include_directories(${PYTHON_INCLUDE_DIR})
    include_directories(ThirdParty/ctpl)
    include_directories(./)

	link_libraries(${GLFW3_LIBRARY})
    link_libraries(${SDL2_IMAGE_LIBRARY})
    link_libraries(${SDL2_LIBRARIES})
    link_libraries(${SDL2_LIBRARY})
	link_libraries(${OPENGL_gl_LIBRARY})
	link_libraries(${GLEW_LIBRARY})
    link_libraries(${PYTHON_LIBRARY})
    
    if(MSVC_VERSION)

        link_libraries(debug ${CURL_LIBRARY_DEBUG}		optimized ${CURL_LIBRARY_RELEASE})
        link_libraries(debug ${TBB_tbb_LIBRARY_DEBUG}	optimized ${TBB_tbb_LIBRARY_RELEASE})
		link_libraries(debug ${ASSIMP_LIBRARY_DEBUG}	optimized ${ASSIMP_LIBRARY_RELEASE})
		#link_libraries(debug ${Boost_FILESYSTEM_LIBRARY_DEBUG}	optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE})
		#link_libraries(debug ${Boost_PYTHON38_LIBRARY_DEBUG}	optimized ${Boost_PYTHON38_LIBRARY_RELEASE})
		#link_libraries(debug ${Boost_RANDOM_LIBRARY_DEBUG}	optimized ${Boost_RANDOM_LIBRARY_RELEASE})

    else()
        
        link_libraries(${TBB_tbb_LIBRARY_RELEASE})
        link_libraries(${CURL_LIBRARY})
        link_libraries(${assimp_LIBRARIES})

    endif()
    
	link_directories(${Boost_LIBRARY_DIRS})

    foreach(assetFilePath ${ASSET_FILES} ${webAsmPlayPython_SRC})
	
		get_filename_component(assetFile ${assetFilePath} NAME)

		if(MSVC_VERSION)
	
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}"		COMMAND cmake -E copy "${assetFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}"			MAIN_DEPENDENCY "${assetFilePath}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${assetFile}" COMMAND cmake -E copy "${assetFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${assetFile}"	MAIN_DEPENDENCY "${assetFilePath}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${assetFile}"		COMMAND cmake -E copy "${assetFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${assetFile}"		MAIN_DEPENDENCY "${assetFilePath}")
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}"			COMMAND cmake -E copy "${assetFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}"			MAIN_DEPENDENCY "${assetFilePath}")
	
			list(APPEND assetFileDestRelease		"${CMAKE_CURRENT_BINARY_DIR}/Release/${assetFile}")
			list(APPEND assetFileDestRelWithDebInfo "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${assetFile}")
			list(APPEND assetFileDestMinSizeRel		"${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${assetFile}")
			list(APPEND assetFileDestDebug			"${CMAKE_CURRENT_BINARY_DIR}/Debug/${assetFile}")
	
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Release/tiles")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/tiles")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/tiles")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Debug/tiles")
	
			file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Release/bubbles")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/bubbles")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/bubbles")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Debug/bubbles")
			
		else()
	
			ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}" "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}" MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/assets/${assetFile}")
	
			list(APPEND assetFileDest "${CMAKE_CURRENT_BINARY_DIR}/${assetFile}")
	
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tiles")
			file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bubbles")
			
		endif()
	
    endforeach(assetFile)
	
    foreach(configFilePath ${config_files})

		get_filename_component(configFile ${configFilePath} NAME)

		if(MSVC_VERSION)

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}"			COMMAND cmake -E copy "${configFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}"			MAIN_DEPENDENCY ${configFilePath})
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}"	COMMAND cmake -E copy "${configFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}"	MAIN_DEPENDENCY ${configFilePath})
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}"		COMMAND cmake -E copy "${configFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}"		MAIN_DEPENDENCY ${configFilePath})
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}"			COMMAND cmake -E copy "${configFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}"				MAIN_DEPENDENCY ${configFilePath})

			list(APPEND configFileDestRelease			"${CMAKE_CURRENT_BINARY_DIR}/Release/${configFile}")
			list(APPEND configFileDestRelWithDebInfo	"${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${configFile}")
			list(APPEND configFileDestMinSizeRel		"${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${configFile}")
			list(APPEND configFileDestDebug				"${CMAKE_CURRENT_BINARY_DIR}/Debug/${configFile}")

		else()

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${configFile}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}" "${CMAKE_CURRENT_BINARY_DIR}/${configFile}" MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/config/${configFile}")

			list(APPEND configFileDest "${CMAKE_CURRENT_BINARY_DIR}/${configFile}")

		endif()

    endforeach(configFile)

	foreach(shaderFilePath ${webAsmPlayShadersGLSL_SRC})

		get_filename_component(shaderFile ${shaderFilePath} NAME)

		if(MSVC_VERSION)

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Release/${shaderFile}"			COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Release/${shaderFile}"			MAIN_DEPENDENCY ${shaderFilePath})
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${shaderFile}"	COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${shaderFile}"	MAIN_DEPENDENCY ${shaderFilePath})
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${shaderFile}"		COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${shaderFile}"		MAIN_DEPENDENCY ${shaderFilePath})
			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Debug/${shaderFile}"			COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/Debug/${shaderFile}"				MAIN_DEPENDENCY ${shaderFilePath})

			list(APPEND shaderFileDestRelease			"${CMAKE_CURRENT_BINARY_DIR}/Release/${shaderFile}")
			list(APPEND shaderFileDestRelWithDebInfo	"${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${shaderFile}")
			list(APPEND shaderFileDestMinSizeRel		"${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel/${shaderFile}")
			list(APPEND shaderFileDestDebug				"${CMAKE_CURRENT_BINARY_DIR}/Debug/${shaderFile}")

		else()

			ADD_CUSTOM_COMMAND(	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${shaderFile}" COMMAND cmake -E copy "${shaderFilePath}" "${CMAKE_CURRENT_BINARY_DIR}/${shaderFile}" MAIN_DEPENDENCY "${shaderFilePath}")

			list(APPEND shaderFileDest "${CMAKE_CURRENT_BINARY_DIR}/${shaderFile}")

		endif()

    endforeach(shaderFilePath)

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

	add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)

	add_library(libWebAsmPlay STATIC ${sources})
	
	add_executable(webAsmPlay "src/Main.cpp" "readme.md")
	target_link_libraries(webAsmPlay libWebAsmPlay)

	add_executable(geoServer ${geoServer_SRC})
    
	target_include_directories(geoServer PRIVATE "ThirdParty/LAStools")
	target_include_directories(geoServer PRIVATE ${EXPAT_INCLUDE_DIRS})

	target_link_libraries(geoServer libWebAsmPlay)
    target_link_libraries(geoServer debug ${EXPAT_LIBRARY_DEBUG} optimized ${EXPAT_LIBRARY_RELEASE})

	if(MSVC_VERSION)

	    message("Compiler is MSVC")

		# Tell MSVC to execute in the build directory of the current configuration.
		set_property(TARGET webAsmPlay PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest /MP")

		set(DLLS_COMMON		${webAsmPlay_BINARY_DIR}/thirdParty/unofficial-flayan-glew.redist.${GLEW_VERSION}/build/native/bin/x64/dynamic/glew32.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/sdl2.new.redist.2.0.8/build/native/bin/x64/SDL2.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libjpeg-9.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libpng16-16.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libtiff-5.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/libwebp-7.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/SDL2_image.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/vii.SDL2_image.redist.2.0.3/build/native/bin/x64/zlib1.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/glfw.${GLFW_VERSION}/build/native/bin/dynamic/v140/x64/glfw3.dll)

        #file(GLOB boost_python_dlls_release ${webAsmPlay_BINARY_DIR}/thirdParty/boost.${BOOST_VERSION}/lib/boost_python37-vc${VC_VERSION}-mt-x64-*.dll
		#									#${webAsmPlay_BINARY_DIR}/thirdParty/boost.${BOOST_VERSION}/lib/boost_python37-vc${VC_VERSION}-mt-x64-*.dll
		#									)
		#
        #file(GLOB boost_python_dlls_debug   ${webAsmPlay_BINARY_DIR}/thirdParty/boost.${BOOST_VERSION}/lib/boost_python37-vc${VC_VERSION}-mt-gd-x64-*.dll)

        set(DLLS_RELEASE	
							${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.redist.9.107.0.0/build/native/bin/x64/Release/tbb.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/expat.v${VC_VERSION}.${EXPAT_VERSION}/build/native/bin/x64/Release/libexpat.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/curl-vc${CURL_VC_VERSION}-dynamic-x86_64.7.59.0/build/native/lib/x64/dynamic/libcurl.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/assimp.v140.redist.3.2/build/native/bin/x64/Release/assimp-vc140-mt.dll
							#${boost_python_dlls_release}
							)

		set(DLLS_DEBUG	
							${webAsmPlay_BINARY_DIR}/thirdParty/tbb_oss.redist.9.107.0.0/build/native/bin/x64/Debug/tbb_debug.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/expat.v${VC_VERSION}.${EXPAT_VERSION}/build/native/bin/x64/Debug/libexpatd.dll
							${webAsmPlay_BINARY_DIR}/thirdParty/curl-vc${CURL_VC_VERSION}-dynamic-x86_64.7.59.0/build/native/lib/x64/dynamic/libcurl_debug.dll
                            ${webAsmPlay_BINARY_DIR}/thirdParty/assimp.v140.redist.3.2/build/native/bin/x64/Debug/assimp-vc140-mtd.dll
                            #${boost_python_dlls_debug}
							)


        foreach(dll ${DLLS_RELEASE} ${DLLS_COMMON})
			file(COPY "${dll}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release")
			file(COPY "${dll}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo")
			file(COPY "${dll}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/MinSizeRel")
        endforeach(dll)
		
        foreach(dll ${DLLS_DEBUG} ${DLLS_COMMON})
            file(COPY "${dll}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
        endforeach(dll)

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
source_group("webAsmPlay\\canvas"			FILES ${webAsmPlayCanvas_SRC})
source_group("webAsmPlay\\geom"				FILES ${webAsmPlayGeom_SRC})
source_group("webAsmPlay\\GUI"				FILES ${webAsmPlayGUI_SRC})
source_group("webAsmPlay\\Bing"				FILES ${webAsmPlayBing_SRC})
source_group("webAsmPlay\\Python"			FILES ${webAsmPlayPython_SRC})







