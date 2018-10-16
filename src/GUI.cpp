#include <cmath>
#include <imgui.h>

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
    #define IMGUI_API
    #include <imgui_impl_glfw_gl3.h>
    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
    #define IMGUI_IMPL_API // What about for windows?
    #include <imgui_impl_opengl3.h>
    #include <imgui_impl_glfw.h>
#endif // __EMSCRIPTEN__

#include <GLFW/glfw3.h>
#include <imgui_internal.h>
#include <iostream>
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
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Renderiable.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tceGeom/vec2.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/RenderiableCollection.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/SkyBox.h>
#include <webAsmPlay/GridPlane.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace geos::geom;
using namespace geos::simplify;
using namespace rsmz;
using namespace glm;
using namespace tce::geom;

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static ImVec4 clear_color = ImColor(114, 144, 154);
static int mouse_buttons_down = 0;

static bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1] = { false, };

GeosTestCanvas  * geosTestCanvas = NULL;
Canvas          * canvas         = NULL;
SkyBox          * skyBox         = NULL;

bool showViewMatrixPanel     = false;
bool showMVP_MatrixPanel     = false;
bool showSceneViewPanel      = false;
bool showPerformancePanel    = false;
bool showRenderSettingsPanel = false;

bool isFirst = true;

FrameBuffer * frameBuffer = NULL;

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void refresh(GLFWwindow* window)
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();

    glfwMarkWindowForRefresh(window);
#endif
}

int counter = 0;

void cback(char* data, int size, void* arg) {
    std::cout << "Callback " << data << " " << size << std::endl;
    counter++;
}

void mainLoop(GLFWwindow * window)
{
    // Game loop
    
#ifdef __EMSCRIPTEN__
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();

    ImGui_ImplGlfwGL3_NewFrame();
#endif

    GeoClient * client = NULL;

    {
        if (ImGui::BeginMainMenuBar())
        {
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

                    client->loadGeometry(canvas);
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

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    // Rendering
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    
    static float time = 0.f;
    
    time += ImGui::GetIO().DeltaTime;

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    if(showPerformancePanel)
    {
        ImGui::Begin("Performance", &showPerformancePanel);

            static float f = 0.0f;
            static float frameTimes[100] = {0.f};
            memcpy(&frameTimes[0], &frameTimes[1], sizeof(frameTimes) - sizeof(frameTimes[0]));
            frameTimes[ARRAYSIZE(frameTimes) - 1] = ImGui::GetIO().Framerate;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PlotLines("Frame History", frameTimes, ARRAYSIZE(frameTimes), 0, "", 0.0f, 100.0f, ImVec2(0, 50));

        ImGui::End();
    }

    if(showViewMatrixPanel)
    {
        ImGui::Begin("View Matrix", &showViewMatrixPanel);

            ImGui::Text(mat4ToStr(canvas->getViewRef()).c_str());

        ImGui::End();
    }
    
    if(isFirst) { ImGui::SetNextWindowPos(ImVec2(0,0)) ;}

    if(showMVP_MatrixPanel)
    {
        ImGui::Begin("MVP Matrix", &showMVP_MatrixPanel);

            ImGui::Text(mat4ToStr(canvas->getMVP_Ref()).c_str());

        ImGui::End();
    }

    isFirst = false;

    if(showRenderSettingsPanel)
    {
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

                for(Renderiable * r : canvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}
            }

            if(renderPolygonOutlines != _renderPolygonOutlines)
            {
                renderPolygonOutlines = _renderPolygonOutlines;

                for(Renderiable * r : canvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}
            }

            if(renderSkyBox != _renderSkyBox)
            {
                renderSkyBox = _renderSkyBox;

                if(renderSkyBox) { canvas->setSkyBox(skyBox) ;}
                else             { canvas->setSkyBox(NULL)   ;}
            }

        ImGui::End();
    }

    canvas->render();

    geosTestCanvas->setEnabled(showSceneViewPanel);

    if(showSceneViewPanel)
    {
        ImGui::Begin("Geos Tests", &showSceneViewPanel);

            const ImVec2 pos = ImGui::GetCursorScreenPos();

            const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

            geosTestCanvas->setArea(__(pos), __(sceneWindowSize));

            geosTestCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

            ImGui::GetWindowDrawList()->AddImage(   (void *)geosTestCanvas->render(),
                                                    pos,
                                                    ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                    ImVec2(0, 1),
                                                    ImVec2(1, 0));
            
            static float buffer1 = 0.1;
            static float buffer2 = 0.02;
            static float buffer3 = 0.22;

            ImGui::SliderFloat("buffer1", &buffer1, 0.0f, 0.3f, "buffer1 = %.3f");
            ImGui::SliderFloat("buffer2", &buffer2, 0.0f, 0.3f, "buffer2 = %.3f");
            ImGui::SliderFloat("buffer3", &buffer3, 0.0f, 0.3f, "buffer3 = %.3f");

            geosTestCanvas->setGeomParameters(buffer1, buffer2, buffer3);

        ImGui::End();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Render();

#ifndef __EMSCRIPTEN__

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif
    glfwMakeContextCurrent(window);

    glfwSwapBuffers(window);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return;
    }

    geosTestCanvas->onMouseButton(window, button, action, mods);

    if(!GImGui->IO.WantCaptureMouse)
    {
        canvas->onMouseButton(window, button, action, mods);
    }

    if (mouse_buttons[button] != action) {
      mouse_buttons[button] = action;
      mouse_buttons_down += action == GLFW_PRESS ? 1 : -1;
    }

#ifndef __EMSCRIPTEN__
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#else
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
#endif
    
    refresh(window);
}

Vec2i lastShiftKeyDownMousePos;

void cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    //dmess("x " << xpos << " y " << ypos);

    geosTestCanvas->onMousePosition(window, Vec2d(xpos, ypos));

    canvas->onMousePosition(window, Vec2d(xpos, ypos));

    {
        refresh(window);
    }
}

void scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    //dmess("ScrollCallback " << xoffset << " " << yoffset);

    geosTestCanvas->onMouseScroll(window, Vec2d(xoffset, yoffset));

    if(!GImGui->IO.WantCaptureMouse)
    {
        canvas->onMouseScroll(window, Vec2d(xoffset, yoffset));
    }

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
#else
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    refresh(window);
}

void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    geosTestCanvas->onKey(window, key, scancode, action, mods);

    if(!GImGui->IO.WantCaptureKeyboard) { canvas->onKey(window, key, scancode, action, mods) ;}
 
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
#else
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif

    refresh(window);
}

void charCallback(GLFWwindow * window, unsigned int c)
{
    geosTestCanvas->onChar(window, c);

    canvas->onChar(window, c);

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_CharCallback(window, c);
#else
    ImGui_ImplGlfw_CharCallback(window, c);
#endif
    refresh(window);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    geosTestCanvas->setArea(Vec2i(0,0), Vec2i(width, height));

    refresh(window);
}

void windowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        refresh(window);
    }
}

void cursorEnterCallback(GLFWwindow * window, int /* entered */)
{
    refresh(window);
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback1(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE) ;}
}

void initOpenGL(GLFWwindow* window)
{
    // Define the viewport dimensions
    static int width, height;
    //glfwGetFramebufferSize(window, &width, &height); 
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    Renderiable::ensureShader();
    GridPlane  ::ensureShader();
    RenderiablePolygon2D::ensureShaders();

    canvas = new Canvas(false);

    dmess("width " << width << " height " << height);

    //canvas->setArea(Vec2i(0,0), Vec2i(width / 2, height / 2));
    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

    geosTestCanvas = new GeosTestCanvas();

    skyBox = new SkyBox();

    canvas->setSkyBox(skyBox);

    GridPlane * gridPlane = new GridPlane();

    canvas->addRenderiable(gridPlane);
}

void initGeometry()
{
    #ifdef WORKING

    Geometry * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Geometry * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    Geometry * ppp = scopedGeosGeometry(GeosUtil::makeBox(-0.05,-0.6,0.05,0.6));

    Geometry * pppp = scopedGeosGeometry(GeosUtil::makeBox(-0.6,-0.05,0.6,0.05));

    p = scopedGeosGeometry(p->buffer(0.1));
    //*

    p = scopedGeosGeometry(p->difference(pp));

    p = scopedGeosGeometry(p->difference(ppp));

    p = scopedGeosGeometry(p->difference(pppp));
    //*/

    const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

    Renderiable * r = Renderiable::create(p, trans);
    //Renderiable * r = Renderiable::create(dynamic_cast<Polygon *>(p)->getExteriorRing());

    r->setFillColor(vec4(0.3,0.3,0,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    //geosTestCanvas->addRenderiable(r);
    canvas->addRenderiable(r);

    /*
    p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,-4,0.4));

    //r = Renderiable::create(p, trans);
    r = Renderiable::create(p);

    r->setFillColor(vec4(0.3,0.0,0.3,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    canvas->addRenderiable(r);
    //*/

    #endif
}
