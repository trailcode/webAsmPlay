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

bool GUI::showViewMatrixPanel        = false;
bool GUI::showMVP_MatrixPanel        = false;
bool GUI::showSceneViewPanel         = false;
bool GUI::showPerformancePanel       = false;
bool GUI::showRenderSettingsPanel    = false;
bool GUI::showLogPanel               = false;
bool GUI::showAttributePanel         = false;
bool GUI::showConfigurationGUI_Panel = false;

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

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
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
        if(ImGui::MenuItem("View Matrix"))     { showViewMatrixPanel        = !showViewMatrixPanel        ;}
        if(ImGui::MenuItem("MVP Matrix"))      { showMVP_MatrixPanel        = !showMVP_MatrixPanel        ;}
        if(ImGui::MenuItem("Geos Tests"))      { showSceneViewPanel         = !showSceneViewPanel         ;}
        if(ImGui::MenuItem("Performance"))     { showPerformancePanel       = !showPerformancePanel       ;}
        if(ImGui::MenuItem("Render Settings")) { showRenderSettingsPanel    = !showRenderSettingsPanel    ;}
        if(ImGui::MenuItem("Log"))             { showLogPanel               = !showLogPanel               ;}
        if(ImGui::MenuItem("Attributes"))      { showAttributePanel         = !showAttributePanel         ;}
        if(ImGui::MenuItem("GUI Settings"))    { showConfigurationGUI_Panel = !showConfigurationGUI_Panel ;}

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

void GUI::configurationGUI_Panel()
{
    if(!showConfigurationGUI_Panel) { return ;}

    ImGuiStyle* ref = NULL;
    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    ImGui::Begin("GUI Configuration", &showConfigurationGUI_Panel);

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

    if (ImGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    ImGui::ShowFontSelector("Fonts##Selector");

    // Simplified Settings
    if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool window_border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
    ImGui::SameLine();
    { bool frame_border = (style.FrameBorderSize > 0.0f); if (ImGui::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
    ImGui::SameLine();
    { bool popup_border = (style.PopupBorderSize > 0.0f); if (ImGui::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    ShowHelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. Use \"Export Colors\" below to save them somewhere.");

    if (ImGui::TreeNode("Rendering"))
    {
        ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); ImGui::SameLine(); ShowHelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
        ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, "%.2f", 2.0f);
        if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
        ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Settings"))
    {
        ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 16.0f, "%.0f");
        ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
        ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
        ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
        ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
        ImGui::Text("BorderSize");
        ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
        ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
        ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
        ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
        ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
        ImGui::Text("Rounding");
        ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
        ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
        ImGui::Text("Alignment");
        ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); ShowHelpMarker("Alignment applies when a button is larger than its text content.");
        ImGui::Text("Safe Area Padding"); ImGui::SameLine(); ShowHelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
        ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Colors"))
    {
        static int output_dest = 0;
        static bool output_only_modified = true;
        if (ImGui::Button("Export Unsaved"))
        {
            if (output_dest == 0)
                ImGui::LogToClipboard();
            else
                ImGui::LogToTTY();
            ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const ImVec4& col = style.Colors[i];
                const char* name = ImGui::GetStyleColorName(i);
                if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                    ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23-(int)strlen(name), "", col.x, col.y, col.z, col.w);
            }
            ImGui::LogFinish();
        }
        ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui::PopItemWidth();
        ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

        ImGui::Text("Tip: Left-click on colored square to open color picker,\nRight-click to open edit options menu.");

        static ImGuiTextFilter filter;
        filter.Draw("Filter colors", 200);

        static ImGuiColorEditFlags alpha_flags = 0;
        ImGui::RadioButton("Opaque", &alpha_flags, 0); ImGui::SameLine();
        ImGui::RadioButton("Alpha", &alpha_flags, ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine();
        ImGui::RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf);

        ImGui::BeginChild("#colors", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
        ImGui::PushItemWidth(-160);
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName(i);
            if (!filter.PassFilter(name))
                continue;
            ImGui::PushID(i);
            ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
            if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
            {
                // Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
                // Read the FAQ and misc/fonts/README.txt about using icon fonts. It's really easy and super convenient!
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i];
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) style.Colors[i] = ref->Colors[i];
            }
            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
            ImGui::TextUnformatted(name);
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::TreePop();
    }

    bool fonts_opened = ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size);
    if (fonts_opened)
    {
        ImFontAtlas* atlas = ImGui::GetIO().Fonts;
        if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
        {
            ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
            ImGui::TreePop();
        }
        ImGui::PushItemWidth(100);
        for (int i = 0; i < atlas->Fonts.Size; i++)
        {
            ImFont* font = atlas->Fonts[i];
            ImGui::PushID(font);
            bool font_details_opened = ImGui::TreeNode(font, "Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
            ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) ImGui::GetIO().FontDefault = font;
            if (font_details_opened)
            {
                ImGui::PushFont(font);
                ImGui::Text("The quick brown fox jumps over the lazy dog");
                ImGui::PopFont();
                ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                ImGui::SameLine(); ShowHelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
                ImGui::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
                ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                ImGui::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)sqrtf((float)font->MetricsTotalSurface), (int)sqrtf((float)font->MetricsTotalSurface));
                for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                    if (ImFontConfig* cfg = &font->ConfigData[config_i])
                        ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                {
                    // Display all glyphs of the fonts in separate pages of 256 characters
                    for (int base = 0; base < 0x10000; base += 256)
                    {
                        int count = 0;
                        for (int n = 0; n < 256; n++)
                            count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
                        if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base+255, count, count > 1 ? "glyphs" : "glyph"))
                        {
                            float cell_size = font->FontSize * 1;
                            float cell_spacing = style.ItemSpacing.y;
                            ImVec2 base_pos = ImGui::GetCursorScreenPos();
                            ImDrawList* draw_list = ImGui::GetWindowDrawList();
                            for (int n = 0; n < 256; n++)
                            {
                                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                                ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                                const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base+n));
                                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255,255,255,100) : IM_COL32(255,255,255,50));
                                if (glyph)
                                    font->RenderChar(draw_list, cell_size, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base+n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
                                {
                                    ImGui::BeginTooltip();
                                    ImGui::Text("Codepoint: U+%04X", base+n);
                                    ImGui::Separator();
                                    ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
                                    ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                    ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        static float window_scale = 1.0f;
        ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
        ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
        ImGui::PopItemWidth();
        ImGui::SetWindowFontScale(window_scale);
        ImGui::TreePop();
    }

    ImGui::PopItemWidth();

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

    configurationGUI_Panel();

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

        //dmess("dddde " << root.hasStringKey(L"showViewMatrixPanel"));

        //root.find(L"string_name") != root.end()

        auto set = [&root](const wstring & key, bool & value)->void
        {
            if(root.find(key) != root.end()) { value = root[key]->AsBool() ;}
        };

        set(L"showViewMatrixPanel",         showViewMatrixPanel);
        set(L"showMVP_MatrixPanel",         showMVP_MatrixPanel);
        set(L"showSceneViewPanel",          showSceneViewPanel);
        set(L"showPerformancePanel",        showPerformancePanel);
        set(L"showPerformancePanel",        showPerformancePanel);
        set(L"showRenderSettingsPanel",     showRenderSettingsPanel);
        set(L"showLogPanel",                showLogPanel);
        set(L"showAttributePanel",          showAttributePanel);
        set(L"showConfigurationGUI_Panel",  showConfigurationGUI_Panel);
    }
}

void GUI::shutdown()
{
    JSONObject root;

    root[L"showViewMatrixPanel"]        = new JSONValue(showViewMatrixPanel);
    root[L"showMVP_MatrixPanel"]        = new JSONValue(showMVP_MatrixPanel);
    root[L"showSceneViewPanel"]         = new JSONValue(showSceneViewPanel);
    root[L"showPerformancePanel"]       = new JSONValue(showPerformancePanel);
    root[L"showRenderSettingsPanel"]    = new JSONValue(showRenderSettingsPanel);
    root[L"showLogPanel"]               = new JSONValue(showLogPanel);
    root[L"showAttributePanel"]         = new JSONValue(showAttributePanel);
    root[L"showConfigurationGUI_Panel"] = new JSONValue(showConfigurationGUI_Panel);

    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    ofstream configFile("config.json");

    configFile << converter.to_bytes(JSONValue(root).Stringify());

    configFile.close();
}