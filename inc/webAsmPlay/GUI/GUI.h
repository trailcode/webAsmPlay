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

#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/Types.h>

//class GLFWwindow;
class Canvas;
class GeosTestCanvas;
class OpenSteerCanvas;
class SkyBox;
class GeoClient;

class GUI
{
public:

    enum
    {
        NORMAL_MODE = 0,
        PICK_MODE_LINESTRING,
        PICK_MODE_POLYGON_SINGLE,
        PICK_MODE_POLYGON_MULTIPLE,
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

    static bool renderSettingsFillMeshes;
    static bool renderSettingsRenderMeshOutlines;
    static bool renderSettingsFillPolygons;
    static bool renderSettingsRenderPolygonOutlines;
    static bool renderSettingsRenderLinearFeatures;
    static bool renderSettingsRenderSkyBox;
    static bool renderSettingsRenderBingMaps;

    static void setupCallbacks(GLFWwindow* window);
    static void initOpenGL();
    static void mainLoop(GLFWwindow * window);
    static void refresh();

    static void loadState();
    
    static void progress(const std::string & message, const float percent);

    static GLFWwindow * getMainWindow();

    static Updatable addUpdatable(Updatable updatable);

    static void shutdown();

    static bool isShuttingDown();

    static float openSteerCameraDist;

    static GeoClient * getClient();

    static void createWorld();

	static void guiASync(std::function<void ()> & callback);

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
    static void GUI_Settings_Panel();
    static void symbologyPanel();
    static void openSteerTestPanel();
    static void openSteerPanel();
    static void cameraInfoPanel();
    static void bingTileSystemPanel();

    static void showHelpMarker(const char* desc);

    static void mouseButtonCallback     (GLFWwindow * window, int button, int action, int mods);
    static void cursorPosCallback       (GLFWwindow * window, double xpos, double ypos);
    static void scrollCallback          (GLFWwindow * window, double xoffset, double yoffset);
    static void keyCallback             (GLFWwindow * window, int key, int scancode, int action, int mods);
    static void charCallback            (GLFWwindow * window, unsigned int c);
    static void framebufferSizeCallback (GLFWwindow * window, int width, int height);
    static void windowFocusCallback     (GLFWwindow * window, int focused);
    static void cursorEnterCallback     (GLFWwindow * window, int entered);
    
    static bool showSceneViewPanel;
    static bool showPerformancePanel;
    static bool showRenderSettingsPanel;
    static bool showLogPanel;
    static bool showAttributePanel;
    static bool showGUI_Settings_Panel;
    static bool showSymbologyPanel;
    static bool showOpenSteerTestPanel;
    static bool showOpenSteerPanel;
    static bool showCameraInfoPanel;
    static bool showBingTileSystemPanel;

    static GLFWwindow * mainWindow;

    static GeosTestCanvas  * geosTestCanvas;
    static OpenSteerCanvas * openSteerCanvas;
    static Canvas          * canvas;
    static SkyBox          * skyBox;
    static GeoClient       * client;

    static std::vector<Canvas *> auxCanvases;

    static int cameraMode;
    
    static bool shuttingDown;

	static EventQueue eventQueue;
};
