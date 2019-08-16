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

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#else
#include <ctpl.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <imguitoolbar.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/canvas/TrackBallInteractor.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/canvas/GeosTestCanvas.h>
#include <webAsmPlay/canvas/OpenSteerCanvas.h>
#include <webAsmPlay/canvas/AnimationCanvas.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/renderables/SkyBox.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/Animation.h>
#include <webAsmPlay/GUI/GUI.h>

// .oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.

//  .-----------------------------------------------------------------.
// /  .-.                                                         .-.  \
//|  /   \                                                       /   \  |
//| |\_.  |                                                     |    /| |
//|\|  | /|                                                     |\  | |/|
//| `---' |                                                     | `---' |
//|       |-----------------------------------------------------|       |
//\       |                                                     |       /
// \     /                                                       \     /
 // `---'                                                         `---'

//  .--.      .-'.      .--.      .--.      .--.      .--.      .`-.      .--.
//:::::.\::::::::.\:Headers.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\
//'      `--'      `.-'      `--'      `--'      `--'      `-.'      `--'      `

using namespace std;
using namespace geos::geom;
using namespace rsmz;
using namespace glm;

//  .--.      .-'.      .--.      .--.      .--.      .--.      .`-.      .--.
//:::::.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\
//'      `--'      `.-'      `--'      `--'      `--'      `-.'      `--'      `

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

GeosTestCanvas  * GUI::s_geosTestCanvas		= NULL;
OpenSteerCanvas * GUI::s_openSteerCanvas	= NULL;
AnimationCanvas * GUI::s_animationCanvas	= NULL;
Canvas          * GUI::s_canvas				= NULL;
SkyBox          * GUI::s_skyBox				= NULL;
GLFWwindow      * GUI::s_mainWindow			= NULL;
int               GUI::s_cameraMode			= GUI::CAMERA_TRACK_BALL;
bool              GUI::s_shuttingDown		= false;
GeoClient       * GUI::s_client				= NULL;
vector<Canvas *>  GUI::s_auxCanvases;
EventQueue		  GUI::s_eventQueue;
bool			  GUI::s_animationRunning	= false;
float			  GUI::s_currAnimationTime	= 0;
float			  GUI::s_animationDuration	= 42.0f;

namespace
{
    bool a_doShowProgressBar = false;

    string a_progressText = "progress";

    float a_progressBarValue = 0.0f;

    ImGuiTextBuffer * a_buf = NULL;

    uint32_t a_infoIcon = 0;

    static char a_mode = GUI::NORMAL_MODE;

    list<Updatable> a_updatables;

#ifndef __EMSCRIPTEN__

    ctpl::thread_pool a_pool(1);

#endif
}

void GUI::refresh()
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();

    glfwMarkWindowForRefresh(mainWindow);

#else

    glfwPostEmptyEvent();

#endif
}

/*
int counter = 0;
void cback(char* data, int size, void* arg) {
    std::cout << "Callback " << data << " " << size << std::endl;
    counter++;
}
*/

struct AppLog
{
    bool ScrollToBottom = true;

    void Clear() { a_buf->clear(); }

    void AddLog(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        va_end(args);
        ScrollToBottom = true;
    }

    void Draw(const char* title, bool* p_opened = NULL)
    {
		ImGui::SetNextWindowSize(ImVec2(500, 400));
        ImGui::Begin(title, p_opened);
        if (ImGui::Button("Clear")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
        if (copy) ImGui::LogToClipboard();
        ImGui::TextUnformatted(a_buf->begin());
        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        //ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }
};

namespace
{
    AppLog logPanel;
}

static void showCursorPositionOverlay(bool* p_open, const dvec4 & cursorPos)
{
    const float DISTANCE = 10.0f;
    static int corner = 2;
    ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    if (corner != -1)
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    if (ImGui::Begin("Cursor Position", p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        /*
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        */
        ImGui::Text("Pos: (%.5f,%.5f)", cursorPos.x, cursorPos.y);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left",     NULL, corner ==  0)) corner =  0;
            if (ImGui::MenuItem("Top-right",    NULL, corner ==  1)) corner =  1;
            if (ImGui::MenuItem("Bottom-left",  NULL, corner ==  2)) corner =  2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner ==  3)) corner =  3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
    
    if(a_buf) { a_buf->appendf("%s %i %s", file.c_str(), (int)line, message.c_str()) ;}
}

void GUI::showProgressBar()
{
    if(!a_doShowProgressBar) { return ;}

    static float progressBarLength = 350.0f;
    ImVec2 window_pos(ImGui::GetIO().DisplaySize.x / 2 - progressBarLength,  ImGui::GetIO().DisplaySize.y / 2);
    ImVec2 window_pos_pivot(0,0);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.6f); // Transparent background
    if (ImGui::Begin(a_progressText.c_str(),
                     &a_doShowProgressBar,    ImGuiWindowFlags_NoTitleBar |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_AlwaysAutoResize |
                                            ImGuiWindowFlags_NoSavedSettings |
                                            ImGuiWindowFlags_NoFocusOnAppearing |
                                            ImGuiWindowFlags_NoNav))
    {
        ImGui::Text(a_progressText.c_str());
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::ProgressBar(a_progressBarValue, ImVec2(progressBarLength,0.0f));
    }
    ImGui::End();
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
void GUI::showHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");

    if(!ImGui::IsItemHovered()) { return ;}

    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}

void GUI::showMainToolBar()
{
    static ImGui::Toolbar toolbar("myFirstToolbar##foo");
    if (toolbar.getNumButtons()==0)
    {
        ImVec2 uv0(0,0);
        ImVec2 uv1(1,1);
        //ImVec2 size(16,16);
        ImVec2 size(32,32);
        toolbar.addButton(ImGui::Toolbutton("Normal Mode",						(void*)a_infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Get Info Linestring Mode",			(void*)a_infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Get Info Polygon Mode",			(void*)a_infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Get Info Polygon Multiple Mode",	(void*)a_infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Set Path Start Point",				(void*)a_infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Find Path",						(void*)a_infoIcon,uv0,uv1,size));

        toolbar.setProperties(false,false,true,ImVec2(0.5f,0.f));

        //toolbar.setScaling(0.5f,0.5f);
    }
    
    const int pressed = toolbar.render();

    switch(pressed)
    {
        case 0: a_mode = NORMAL_MODE;                 break;
        case 1: a_mode = PICK_MODE_LINESTRING;        break;
        case 2: a_mode = PICK_MODE_POLYGON_SINGLE;    break;
        case 3: a_mode = PICK_MODE_POLYGON_MULTIPLE;  break;
        case 4: a_mode = SET_PATH_START_POINT;        break;
        case 5: a_mode = FIND_PATH;                   break;
    }
}

namespace
{
	ivec2 _wndPos;
	ivec2 _wndSize;
}

bool g_fullScreen = false;

void setFullScreen( bool fullscreen )
{
	//if ( IsFullscreen() == fullscreen )
		//return;
	g_fullScreen = fullscreen;

	if ( fullscreen )
	{
		// backup windwo position and window size
		glfwGetWindowPos ( GUI::getMainWindow(), &_wndPos[0],  &_wndPos[1]  );
		glfwGetWindowSize( GUI::getMainWindow(), &_wndSize[0], &_wndSize[1] );

		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		dmess("count " << count);

		// get reolution of monitor
		const GLFWvidmode * mode = glfwGetVideoMode(monitors[1]);

		// switch to full screen
		//glfwSetWindowMonitor( GUI::getMainWindow(), glfwGetPrimaryMonitor(), 0, 0, 1280 * 2, 720 * 2, 0 );
		glfwSetWindowMonitor( GUI::getMainWindow(), monitors[1], 0, 0, mode->width, mode->height, mode->refreshRate);

		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(1);

		// restore last window size and position
		glfwSetWindowMonitor( GUI::getMainWindow(), nullptr,  _wndPos[0], _wndPos[1], _wndSize[0], _wndSize[1], 0);
	}

	//_updateViewport = true;
}

void GUI::showMainMenuBar()
{
    if (!ImGui::BeginMainMenuBar()) { return ;}

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open", "Ctrl+O")) {

        }
        if(ImGui::MenuItem("Test Web Worker"))
        {
            #ifdef __EMSCRIPTEN__
                //worker_handle worker = emscripten_create_worker("worker.js");
                //emscripten_call_worker(worker, "one", 0, 0, cback, (void*)42);
            #else
                dmess("Implement me!");
            #endif
        }

        if(ImGui::MenuItem("Test Emscripten Fetch"))
        {
             #ifdef __EMSCRIPTEN__

             #endif
        }

        if(ImGui::MenuItem("Load Geometry")) { s_client->loadGeoServerGeometry() ;}

		if (ImGui::MenuItem("Exit"))
		{

			exit(0);
		}

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        ImGui::Separator();
        if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("View"))
    {
        if (ImGui::MenuItem("Geos Tests"))				{ s_showSceneViewPanel					^= 1 ;}
        if (ImGui::MenuItem("Performance"))				{ s_showPerformancePanel				^= 1 ;}
        if (ImGui::MenuItem("Render Settings"))			{ s_showRenderSettingsPanel				^= 1 ;}
        if (ImGui::MenuItem("Log"))						{ s_showLogPanel						^= 1 ;}
        if (ImGui::MenuItem("Attributes"))				{ s_showAttributePanel					^= 1 ;}
        if (ImGui::MenuItem("GUI Settings"))			{ s_showGUI_Settings_Panel				^= 1 ;}
        if (ImGui::MenuItem("Symbology"))				{ s_showSymbologyPanel					^= 1 ;}
        if (ImGui::MenuItem("OpenSteer Test"))			{ s_showOpenSteerTestPanel				^= 1 ;}
        if (ImGui::MenuItem("OpenSteer"))				{ s_showOpenSteerPanel					^= 1 ;}
        if (ImGui::MenuItem("Camera Info"))				{ s_showCameraInfoPanel					^= 1 ;}
        if (ImGui::MenuItem("Bing Tile System"))		{ s_showBingTileSystemPanel				^= 1 ;}
		if (ImGui::MenuItem("Framebuffer Depth"))		{ s_showFrameBufferDepthDebugPanel		^= 1 ;}
		if (ImGui::MenuItem("BingMaps Framebuffer"))	{ s_showBingMapsFrameBufferDebugPanel	^= 1 ;}
		if (ImGui::MenuItem("Normal Framebuffer"))		{ s_showNormalFrameBufferDebugPanel		^= 1 ;}
		if (ImGui::MenuItem("Animation"))				{ s_showAnimationPanel					^= 1 ;}
		if (ImGui::MenuItem("Full Screen"))				{ setFullScreen(!g_fullScreen) ;}

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void GUI::performacePanel()
{
    if(!s_showPerformancePanel) { return ;}
    
    ImGui::Begin("Performance", &s_showPerformancePanel);

        static float f = 0.0f;
        static float frameTimes[100] = {0.f};
        memmove(&frameTimes[0], &frameTimes[1], sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[ARRAYSIZE(frameTimes) - 1] = ImGui::GetIO().Framerate;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::PlotLines("Frame History", frameTimes, ARRAYSIZE(frameTimes), 0, "", 0.0f, 100.0f, ImVec2(0, 50));

    ImGui::End();
}

void GUI::attributePanel(const string & attrsStr)
{
    if(!s_showAttributePanel) { return ;}

    ImGui::Begin("Attributes", &s_showAttributePanel);

        ImGui::Text(attrsStr.c_str());

    ImGui::End();
}

namespace
{
	const thread::id mainThreadID = this_thread::get_id();
}

bool GUI::isMainThread() { return this_thread::get_id() == mainThreadID ;}

void GUI::doQueue()
{
	function<void()> * _f = nullptr;

	if (s_eventQueue.try_pop(_f))
	{
		(*_f)();
		
		delete _f;
	}
}

void GUI::mainLoop(GLFWwindow * window)
{
    if(!a_buf) {  a_buf = new ImGuiTextBuffer() ;}
    // Game loop
    
    //dmess("mainLoop");

#ifdef __EMSCRIPTEN__
    // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif

    static bool opt_fullscreen_persistant = true;
    static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
    bool opt_fullscreen = opt_fullscreen_persistant;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        
    }

	opt_flags |= ImGuiDockNodeFlags_PassthruCentralNode;

    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", NULL, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2); // Full screen

    // Dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
    
    // Rendering
    int screenWidth, screenHeight;

    glfwGetFramebufferSize(s_mainWindow, &screenWidth, &screenHeight);
    
    glViewport(0, 0, screenWidth, screenHeight);
    
    static float time = 0.f;
    
    time += ImGui::GetIO().DeltaTime;

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
 
    s_canvas->render();
    
    const double dist = distance(s_canvas->getCamera()->getCenter(), s_canvas->getCamera()->getEye());

    s_canvas->getTrackBallInteractor()->setZoomScale(dist * 0.02);

    const dvec4 pos(s_canvas->getCursorPosWC(), 1.0);

    showCursorPositionOverlay(NULL, s_client->getInverseTrans() * pos);
    //showCursorPositionOverlay(NULL, pos);

    string attrsStr = s_client->doPicking(a_mode, pos); // TODO move to a_updatables

    for(auto & i : a_updatables) { i() ;}

    if(s_showLogPanel) { logPanel.Draw("Log", &s_showLogPanel) ;}

    showMainToolBar();
    showMainMenuBar();
    showProgressBar();
    GUI_Settings_Panel();
    performacePanel();
    renderSettingsPanel();
    symbologyPanel();
    attributePanel(attrsStr);
    geosTestPanel();
    openSteerTestPanel();
    openSteerPanel();
    cameraInfoPanel();
    bingTileSystemPanel();
	frameBufferDepthDebugPanel();
	bingMapsFrameBufferDebugPanel();
	normalFrameBufferDebugPanel();
	animationPanel();

	//ImGui::ShowDemoWindow();

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();

    glfwMakeContextCurrent(s_mainWindow);

    glfwSwapBuffers(s_mainWindow);

	if (s_animationRunning)
	{
		s_currAnimationTime += ImGui::GetIO().DeltaTime;

		if (s_currAnimationTime > s_animationDuration) { s_animationDuration = s_currAnimationTime; }

		Animation::update(s_currAnimationTime);
	}

#ifdef __EMSCRIPTEN__

    refresh();

#endif

	
}

void GUI::progress(const string & message, const float percent)
{
    if(percent >= 1.0)
    {
        a_doShowProgressBar = false;

        return;
    }

    a_doShowProgressBar = true;

    a_progressText = message;

    a_progressBarValue = percent;
}

void GUI::initOpenGL() // TODO, need some code refactor here
{
    debugLoggerFunc = &dmessCallback;

    a_infoIcon = Textures::load("if_Info_131908.png");
    
    // Define the viewport dimensions
    static int width, height; 
    int fbWidth, fbHeight;

    glfwGetWindowSize		(s_mainWindow, &width,	 &height);
    glfwGetFramebufferSize	(s_mainWindow, &fbWidth, &fbHeight);

    glViewport(0, 0, fbWidth, fbHeight); // TODO needed?

	OpenGL::init();

    s_canvas = new Canvas(false);

    s_canvas->setFrameBufferSize(ivec2(fbWidth, fbHeight));

	dmess("fbWidth " << fbWidth << " width " << width);

    s_auxCanvases = vector<Canvas *>(
    {
        s_geosTestCanvas  = new GeosTestCanvas(),
        s_openSteerCanvas = new OpenSteerCanvas(),
		s_animationCanvas = new AnimationCanvas()
    });
}

Updatable GUI::addUpdatable(Updatable updatable)
{
    a_updatables.push_back(updatable);

    return updatable;
}

void GUI::createWorld()
{
    s_skyBox = new SkyBox();

    if(s_renderSettingsRenderSkyBox) { s_canvas->setSkyBox(s_skyBox) ;} // TODO create check render functor
    else                             { s_canvas->setSkyBox(NULL)     ;}

    a_pool.push([](int ID) {
        
		OpenGL::ensureSharedContext();

		s_client = new GeoClient(s_canvas);

        //client->loadGeometry("https://trailcode.github.io/ZombiGeoSim/data.geo");
        s_client->loadGeometry("data.geo");
    });
}

int GUI::getCameraMode() { return s_cameraMode ;}

void GUI::shutdown()
{
    saveState();

    s_shuttingDown = true;
}

bool GUI::isShuttingDown() { return s_shuttingDown ;}

char GUI::getMode() { return a_mode ;}

GLFWwindow * GUI::getMainWindow() { return s_mainWindow ;}

GeoClient * GUI::getClient() { return s_client ;}

Canvas * GUI::getMainCanvas() { return s_canvas ;}

Camera * GUI::getMainCamera() { return s_canvas->getCamera() ;}

TrackBallInteractor * GUI::getTrackBallInteractor() { return getMainCanvas()->getTrackBallInteractor() ;}


