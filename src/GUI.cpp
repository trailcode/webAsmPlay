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

#include <cmath>
#include <memory>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <locale>
#include <codecvt>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/simplify/TopologyPreservingSimplifier.h>
#include <geos/simplify/DouglasPeuckerSimplifier.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imguitoolbar.h>
#include <JSON.h>
#include <webAsmPlay/ImguiInclude.h>
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/SkyBox.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/GridPlane.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/GUI.h>

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
using namespace geos::simplify;
using namespace rsmz;
using namespace glm;

//  .--.      .-'.      .--.      .--.      .--.      .--.      .`-.      .--.
//:::::.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\::::::::.\
//'      `--'      `.-'      `--'      `--'      `--'      `-.'      `--'      `

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static ImVec4 clear_color = ImColor(114, 144, 154);
static int mouse_buttons_down = 0;

static bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1] = { false, };

GeosTestCanvas  * GUI::geosTestCanvas = NULL;
Canvas          * GUI::canvas         = NULL;
SkyBox          * GUI::skyBox         = NULL;

bool GUI::showViewMatrixPanel     = false;
bool GUI::showMVP_MatrixPanel     = false;
bool GUI::showSceneViewPanel      = false;
bool GUI::showPerformancePanel    = false;
bool GUI::showRenderSettingsPanel = false;
bool GUI::showLogPanel            = false;
bool GUI::showAttributePanel      = false;

bool isFirst = true;

FrameBuffer * frameBuffer = NULL;

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void GUI::refresh(GLFWwindow* window)
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();

    //dmess("Fix!");
    glfwMarkWindowForRefresh(window);
#else

    glfwPostEmptyEvent();

#endif
}

int counter = 0;

void cback(char* data, int size, void* arg) {
    std::cout << "Callback " << data << " " << size << std::endl;
    counter++;
}

ImGuiTextBuffer * Buf = NULL;

uint32_t infoIcon = 0;

struct AppLog
{
    //ImGuiTextBuffer     Buf;
    bool                ScrollToBottom;

    void    Clear()     { Buf->clear(); }

    void    AddLog(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        //Buf.appendv(fmt, args);
        //Buf.appendf("%s", "fdasdfasd");
        va_end(args);
        ScrollToBottom = true;
    }

    void Draw(const char* title, bool* p_opened = NULL)
    {
        ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiSetCond_FirstUseEver);
        ImGui::Begin(title, p_opened);
        if (ImGui::Button("Clear")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
        if (copy) ImGui::LogToClipboard();
        ImGui::TextUnformatted(Buf->begin());
        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }
};

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

AppLog logPanel;

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
    
    if(Buf) { Buf->appendf("%s %i %s", file.c_str(), (int)line, message.c_str()) ;}
}

GeoClient * client = NULL;

static char mode = GUI::NORMAL_MODE;

void GUI::showMainToolBar()
{
    static ImGui::Toolbar toolbar("myFirstToolbar##foo");
    if (toolbar.getNumButtons()==0)
    {
        ImVec2 uv0(0,0);
        ImVec2 uv1(1,1);
        //ImVec2 size(16,16);
        ImVec2 size(32,32);
        toolbar.addButton(ImGui::Toolbutton("Normal Mode",(void*)infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Get Info Linestring Mode",(void*)infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Get Info Polygon Mode",(void*)infoIcon,uv0,uv1,size));
        toolbar.addButton(ImGui::Toolbutton("Get Info Polygon Multiple Mode",(void*)infoIcon,uv0,uv1,size));

        toolbar.setProperties(false,false,true,ImVec2(0.5f,0.f));

        //toolbar.setScaling(0.5f,0.5f);
    }
    
    const int pressed = toolbar.render();
    if (pressed>=0) fprintf(stderr,"Toolbar1: pressed:%d\n",pressed);
    switch(pressed)
    {
        case 0: mode = NORMAL_MODE; break;
        case 1: mode = PICK_MODE_LINESTRING; break;
        case 2: mode = PICK_MODE_POLYGON_SINGLE; break;
        case 3: mode = PICK_MODE_POLYGON_MULTIPLE; break;
    }
}

void GUI::showMainMenuBar(GLFWwindow * window)
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
                worker_handle worker = emscripten_create_worker("worker.js");
                emscripten_call_worker(worker, "one", 0, 0, cback, (void*)42);
            #else
                dmess("Implement me!");
            #endif
        }

        if(ImGui::MenuItem("Load Geometry"))
        {
            //GeoClient::getInstance()->loadGeometry(canvas);

            if(!client) { client = new GeoClient(window) ;}

            client->loadAllGeometry(canvas);
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
        if(ImGui::MenuItem("View Matrix"))     { showViewMatrixPanel     = !showViewMatrixPanel     ;}
        if(ImGui::MenuItem("MVP Matrix"))      { showMVP_MatrixPanel     = !showMVP_MatrixPanel     ;}
        if(ImGui::MenuItem("Geos Tests"))      { showSceneViewPanel      = !showSceneViewPanel      ;}
        if(ImGui::MenuItem("Performance"))     { showPerformancePanel    = !showPerformancePanel    ;}
        if(ImGui::MenuItem("Render Settings")) { showRenderSettingsPanel = !showRenderSettingsPanel ;}
        if(ImGui::MenuItem("Log"))             { showLogPanel            = !showLogPanel            ;}
        if(ImGui::MenuItem("Attributes"))      { showAttributePanel      = !showAttributePanel      ;}

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void GUI::performacePanel()
{
    if(!showPerformancePanel) { return ;}
    
    ImGui::Begin("Performance", &showPerformancePanel);

        static float f = 0.0f;
        static float frameTimes[100] = {0.f};
        memcpy(&frameTimes[0], &frameTimes[1], sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[ARRAYSIZE(frameTimes) - 1] = ImGui::GetIO().Framerate;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::PlotLines("Frame History", frameTimes, ARRAYSIZE(frameTimes), 0, "", 0.0f, 100.0f, ImVec2(0, 50));

    ImGui::End();
}

void GUI::viewMatrixPanel()
{
    if(!showViewMatrixPanel) { return ;}

    ImGui::Begin("View Matrix", &showViewMatrixPanel);

        ImGui::Text(mat4ToStr(canvas->getViewRef()).c_str());

    ImGui::End();
}

void GUI::MVP_MatrixPanel()
{
    if(!showMVP_MatrixPanel) { return ;}
    
    ImGui::Begin("MVP Matrix", &showMVP_MatrixPanel);

        ImGui::Text(mat4ToStr(canvas->getMVP_Ref()).c_str());

    ImGui::End();
}

void GUI::renderSettingsPanel()
{
    if(!showRenderSettingsPanel) { return ;}

    ImGui::Begin("Render Settings", &showRenderSettingsPanel);

        static bool fillPolygons            = true;
        static bool renderPolygonOutlines   = true;
        static bool renderSkyBox            = true;

        static bool _fillPolygons           = true;
        static bool _renderPolygonOutlines  = true;
        static bool _renderSkyBox           = true;

        ImGui::Checkbox("Fill Polygons",    &_fillPolygons);
        ImGui::Checkbox("Polygon Outlines", &_renderPolygonOutlines);
        ImGui::Checkbox("SkyBox",           &_renderSkyBox);

        if(fillPolygons != _fillPolygons)
        {
            fillPolygons = _fillPolygons;

            for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

            for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

            Renderable::setDefaultRenderFill(fillPolygons);
        }

        if(renderPolygonOutlines != _renderPolygonOutlines)
        {
            renderPolygonOutlines = _renderPolygonOutlines;

            for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

            for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

            Renderable::setDefaultRenderOutline(renderPolygonOutlines);
        }

        if(renderSkyBox != _renderSkyBox)
        {
            renderSkyBox = _renderSkyBox;

            if(renderSkyBox) { canvas->setSkyBox(skyBox) ;}
            else             { canvas->setSkyBox(NULL)   ;}
        }

        static vec4 fillColor(Renderable::getDefaultFillColor());
        static vec4 outlineColor(Renderable::getDefaultOutlineColor());
        
        //ImGui::ColorEdit4("Fill", (float*)&fillColor, true);
        //ImGui::ColorEdit4("Outline", (float*)&outlineColor, true);

        
        ImGui::ColorPicker4("##picker",
                            (float*)&fillColor,
                            //ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                            0);

    ImGui::End();
}

void GUI::attributePanel(const string & attrsStr)
{
    if(!showAttributePanel) { return ;}

    ImGui::Begin("Attributes", &showAttributePanel);

        ImGui::Text(attrsStr.c_str());

    ImGui::End();
}

void GUI::mainLoop(GLFWwindow * window)
{
    if(!Buf) {  Buf = new ImGuiTextBuffer() ;}
    // Game loop
    
    //dmess("mainLoop");

#ifdef __EMSCRIPTEN__
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
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

    opt_flags |= ImGuiDockNodeFlags_PassthruDockspace;

    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", NULL, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2); // Full screen

    // Dockspace
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
    
    showMainToolBar();

    showMainMenuBar(window);

    // Rendering
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    
    static float time = 0.f;
    
    time += ImGui::GetIO().DeltaTime;

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    performacePanel();

    viewMatrixPanel();

    //if(isFirst) { ImGui::SetNextWindowPos(ImVec2(0,0)) ;}

    MVP_MatrixPanel();

    isFirst = false;

    renderSettingsPanel();

    if(showLogPanel) { logPanel.Draw("Log", &showLogPanel) ;}

    canvas->render();

    const double dist = distance(canvas->getCamera()->getCenter(), canvas->getCamera()->getEye());

    const double scale = canvas->getTrackBallInteractor()->mZoomScale = dist * 0.02;

    string attrsStr;

    if(client)
    {
        const dvec4 pos(canvas->getCursorPosWC(), 1.0);

        showCursorPositionOverlay(NULL, client->getInverseTrans() * pos);

        attrsStr = client->doPicking(mode, pos, canvas);
    }

    attributePanel(attrsStr);

    sceneViewPanel();

    ImGui::End();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();

    glfwMakeContextCurrent(window);

    glfwSwapBuffers(window);
}

void GUI::initOpenGL(GLFWwindow* window)
{
    debugLoggerFunc = &dmessCallback;

    infoIcon = Textures::load("if_Info_131908.png");
    //if(!infoIcon) { infoIcon = Textures::load("if_Info_131908.png") ;}
    //if(!infoIcon) { infoIcon = Textures::load("xpos.png") ;}

    // Define the viewport dimensions
    static int width, height;
    
    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    Renderable::ensureShader();
    //GridPlane  ::ensureShader();
    RenderablePolygon::ensureShaders();

    canvas = new Canvas(false);

    canvas->setArea(ivec2(0,0), ivec2(width, height));

    geosTestCanvas = new GeosTestCanvas();

    skyBox = new SkyBox();

    canvas->setSkyBox(skyBox);

    //GridPlane * gridPlane = new GridPlane();

    //canvas->addRenderiable(gridPlane);

    /*
    FILE * fp = fopen("Config.ini");

    if(fp)
    {
        fread(&showViewMatrixPanel;
        fread(&showMVP_MatrixPanel;
        fread(&showSceneViewPanel;
        fread(&showPerformancePanel;
        fread(&showRenderSettingsPanel;
        fread(&showLogPanel;
        fread(&showAttributePanel;
    }

    fclose(fp);
    */

    ifstream configFile("config.json");
    
    const string config((istreambuf_iterator<char>(configFile)), istreambuf_iterator<char>());

    dmess("config " << config);

    JSONValue * value = JSON::Parse(config.c_str());

    if(value)
    {
        JSONObject root = value->AsObject();

        showViewMatrixPanel     = root[L"showViewMatrixPanel"]->AsBool();
        showMVP_MatrixPanel     = root[L"showMVP_MatrixPanel"]->AsBool();
        showSceneViewPanel      = root[L"showSceneViewPanel"]->AsBool();
        showPerformancePanel    = root[L"showPerformancePanel"]->AsBool();
        showRenderSettingsPanel = root[L"showRenderSettingsPanel"]->AsBool();
        showLogPanel            = root[L"showLogPanel"]->AsBool();
        showAttributePanel      = root[L"showAttributePanel"]->AsBool();
    }
}

void GUI::shutdown()
{
    JSONObject root;

    root[L"showViewMatrixPanel"]     = new JSONValue(showViewMatrixPanel);
    root[L"showMVP_MatrixPanel"]     = new JSONValue(showMVP_MatrixPanel);
    root[L"showSceneViewPanel"]      = new JSONValue(showSceneViewPanel);
    root[L"showPerformancePanel"]    = new JSONValue(showPerformancePanel);
    root[L"showRenderSettingsPanel"] = new JSONValue(showRenderSettingsPanel);
    root[L"showLogPanel"]            = new JSONValue(showLogPanel);
    root[L"showAttributePanel"]      = new JSONValue(showAttributePanel);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    ofstream configFile("config.json");

    configFile << converter.to_bytes(JSONValue(root).Stringify());

    configFile.close();
}