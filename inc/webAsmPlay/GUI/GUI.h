/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/
#pragma once

#include <future>
#include <chrono>
#include <thread>
#include <ctpl/ctpl.h>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Types.h>

class Canvas;
class GeosTestCanvas;
class KD_TreeTestCanvas;
class BubbleFaceTestCanvas;
class OpenSteerCanvas;
class AnimationCanvas;
class ModelViewerCanvas;
class SkyBox;
class GeoClient;

namespace rsmz
{
	class Camera;
	class TrackBallInteractor;
}

class GUI
{
public:

    enum
    {
        NORMAL_MODE = 0,
        PICK_MODE_LINESTRING,
        PICK_MODE_POLYGON_SINGLE,
        PICK_MODE_POLYGON_MULTIPLE,
        PICK_MODE_POINT,
        PICK_MODE_POLYGON_CHILDREN,
        SET_PATH_START_POINT,
        FIND_PATH,
    };

    enum
    {
        CAMERA_TRACK_BALL = 0,
        CAMERA_FIRST_PERSON,
        CAMERA_FOLLOW_ENTITY,
    };

    static char getMode();

    static int getCameraMode();

    static bool  s_renderSettingsFillMeshes;
    static bool  s_renderSettingsRenderMeshOutlines;
    static bool  s_renderSettingsFillPolygons;
    static bool  s_renderSettingsRenderPolygonOutlines;
    static bool  s_renderSettingsRenderLinearFeatures;
    static bool  s_renderSettingsRenderSkyBox;
    static bool  s_renderSettingsRenderBingMaps;
    static bool  s_renderSettingsRenderModels;
	static float s_lineWidthRender;

    static void setupCallbacks(GLFWwindow* window);
    static void initOpenGL();
	static void initBingTileSystemPanel(const glm::dmat4 & trans);
	static void initBingStreetSidePanel(const glm::dmat4 & trans);
	static void initBubbleFacePanels();
	static void initRenderSettingsPanel();
    static void mainLoop(GLFWwindow * window);
    static void refresh();

    static void loadState();
    
    static void progress(const std::string & message, const float percent);

    static GLFWwindow * getMainWindow();

    static Updatable addUpdatable(Updatable updatable);

    static void shutdown();

    static bool isShuttingDown();

    static float s_openSteerCameraDist;

    static GeoClient * getClient();

	static Canvas * getMainCanvas();

	static rsmz::Camera * getMainCamera();

	static rsmz::TrackBallInteractor * getTrackBallInteractor();

    static void createWorld();

	static bool isMainThread();

	template<typename F>
	static auto guiASync(F && f) ->std::future<decltype(f())>
	{
		auto pck = std::make_shared<std::packaged_task<decltype(f())()>>(std::forward<F>(f));

		auto _f = new std::function<void()>([pck]() { (*pck)() ;});

#ifndef __EMSCRIPTEN__

		s_eventQueue.push(_f);

#else

		dmessError("Implement me!");

#endif

		return pck->get_future();
	}

	template<typename F>
	static auto guiSync(F && f, const bool executeDirectlyIfMainThread = true) ->std::future<decltype(f())>
	{
		auto pck = std::make_shared<std::packaged_task<decltype(f())()>>(std::forward<F>(f));

		if (executeDirectlyIfMainThread && isMainThread())
		{
			f();

			return pck->get_future();
		}

		auto _f = new std::function<void()>([pck]() { (*pck)() ;});

#ifndef __EMSCRIPTEN__

		s_eventQueue.push(_f);

		while (!s_eventQueue.empty()) { std::this_thread::sleep_for(std::chrono::milliseconds(10)) ;}

#else

		dmessError("Implement me!");

#endif

		return pck->get_future();
	}

	template<typename F, typename... Rest>
	static auto queue(F && f, Rest&&... rest) ->std::future<decltype(f(0, rest...))>
	{
#ifndef __EMSCRIPTEN__
		return s_generalWorkPool.push(f, rest...);
#endif
		dmessError("Implement me!");
	}

	static void doQueue();

	static bool s_animationRunning;

	static float s_currAnimationTime;
	static float s_animationDuration;

private:

    GUI() {}
    ~GUI() {}

    static void saveState();

    static void showMainToolBar();
    static void showMainMenuBar();
    static void showProgressBar();
    static void performacePanel();
    static void MVP_MatrixPanel();
    static void renderSettingsPanel();
    static void attributePanel(const std::string & attrsStr);
    static void geosTestPanel();
	static void KD_TreeTestPanel();
	static void boostGeomTestPanel();
    static void GUI_Settings_Panel();
    static void symbologyPanel();
    static void openSteerTestPanel();
    static void openSteerPanel();
    static void cameraInfoPanel();
    static void bingTileSystemPanel();
	static void frameBufferDepthDebugPanel();
	static void bingMapsFrameBufferDebugPanel();
	static void normalFrameBufferDebugPanel();
	static void animationPanel();
	static void modelViewerPanel();
	static void streetSidePanel();
	static void bubbleFaceTestPanel();
	static void bubbleFacePanels();
	static void textureSystemPanel();
	static void pythonConsolePanel();
	static void solidNodeBSP_Panel();

    static void showHelpMarker(const char* desc);

    static void mouseButtonCallback     (GLFWwindow * window, int button, int action, int mods);
    static void cursorPosCallback       (GLFWwindow * window, double xpos, double ypos);
    static void scrollCallback          (GLFWwindow * window, double xoffset, double yoffset);
    static void keyCallback             (GLFWwindow * window, int key, int scancode, int action, int mods);
    static void charCallback            (GLFWwindow * window, unsigned int c);
    static void framebufferSizeCallback (GLFWwindow * window, int width, int height);
    static void windowFocusCallback     (GLFWwindow * window, int focused);
    static void cursorEnterCallback     (GLFWwindow * window, int entered);
    
    static bool s_showGeosTestPanel;
	static bool s_showKD_TreeTestPanel;
	static bool s_showBoostGeomTestPanel;
    static bool s_showPerformancePanel;
    static bool s_showRenderSettingsPanel;
    static bool s_showLogPanel;
    static bool s_showAttributePanel;
    static bool s_showGUI_Settings_Panel;
    static bool s_showSymbologyPanel;
    static bool s_showOpenSteerTestPanel;
    static bool s_showOpenSteerPanel;
    static bool s_showCameraInfoPanel;
    static bool s_showBingTileSystemPanel;
	static bool s_showFrameBufferDepthDebugPanel;
	static bool s_showBingMapsFrameBufferDebugPanel;
	static bool s_showNormalFrameBufferDebugPanel;
	static bool s_showAnimationPanel;
	static bool s_showModelViewerPanel;
	static bool s_showStreetSidePanel;
	static bool s_showBubbleFaceTestPanel;
	static bool s_showBubbleFacePanel[6];
	static bool s_showTextureSystemPanel;
	static bool s_showPythonConsolePanel;
	static bool s_showSolidNodeBSP_Panel;

    static GLFWwindow * s_mainWindow;

    static GeosTestCanvas		* s_geosTestCanvas;
	static KD_TreeTestCanvas	* s_KD_TreeTestCanvas;
    static OpenSteerCanvas		* s_openSteerCanvas;
	static AnimationCanvas		* s_animationCanvas;
	static ModelViewerCanvas	* s_modelViewerCanvas;
	static BubbleFaceTestCanvas * s_bubbleFaceTestCanvas;
    static Canvas				* s_canvas;
    static SkyBox				* s_skyBox;
    static GeoClient			* s_client;

    static std::vector<Canvas *> s_auxCanvases;

    static int s_cameraMode;
    
    static bool s_shuttingDown;

#ifndef __EMSCRIPTEN__

	static EventQueue s_eventQueue;

	static ctpl::thread_pool<boost::lockfree::queue<std::function<void(int id)> *>> s_generalWorkPool;

#endif
};
