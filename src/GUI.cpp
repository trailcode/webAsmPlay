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
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace geos::geom;
using namespace rsmz;
using namespace glm;
using namespace tce::geom;

//#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

static ImVec4 clear_color = ImColor(114, 144, 154);
static int mouse_buttons_down = 0;

static bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1] = { false, };

Canvas * auxCanvas = NULL;
Canvas * canvas = NULL;

bool showViewMatrixPanel = false;
bool showMVP_MatrixPanel = false;
bool showSceneViewPanel  = true;

bool isFirst = true;

FrameBuffer * frameBuffer = NULL;
ImVec2 sceneWindowSize;

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void cleanup()
{
    // Properly de-allocate all resources once they've outlived their purpose
    glfwTerminate();
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

void mainLoop(GLFWwindow* window)
{
    // Game loop
    if (glfwWindowShouldClose(window)) {
      cleanup();
      return;
    }

#ifdef __EMSCRIPTEN__
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();

    ImGui_ImplGlfwGL3_NewFrame();
#endif

    bool doIt = false;
    bool doTest7 = false;

    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                //ShowExampleMenuFile();
                //ImGui::MenuItem("(dummy menu)", NULL, false, false);
                if (ImGui::MenuItem("New")) {}
                if (ImGui::MenuItem("Open", "Ctrl+O")) {

                    cout << "Open" << endl;

                    worker_handle worker = emscripten_create_worker("worker.js");
                    emscripten_call_worker(worker, "one", 0, 0, cback, (void*)42);

                    //emscripten_set_main_loop(loop, 2, true);

                    //emscripten_run_script("fileInput.click();");
                    //emscripten_run_script("alert(fileSelector)");
                    emscripten_run_script("Module.my_js();");

                    //my_c_function(0);

                    //EM_ASM("Module.my_js();");

                    //my_js();


                }
                if(ImGui::MenuItem("Test Web Worker"))
                {
                    worker_handle worker = emscripten_create_worker("worker.js");
                    emscripten_call_worker(worker, "one", 0, 0, cback, (void*)42);
                }
                if(ImGui::MenuItem("Test"))
                {
                    emscripten_run_script("Module.connection.send(\"\\0\");");
                }

                if(ImGui::MenuItem("Test 2"))
                {
                    doIt = true;
                    
                }

                if(ImGui::MenuItem("Test 3"))
                {
                    emscripten_run_script("Module.connection.send(\"\\4\");");
                }

                if(ImGui::MenuItem("Test 4"))
                {
                    GeoClient::getInstance()->getNumGeoms([](const size_t numGeoms)
                    {
                        dmess("numGeoms " << numGeoms);
                    });
                }

                if(ImGui::MenuItem("Test 5"))
                {
                    GeoClient::getInstance()->getLayerBounds([](const AABB2D & bounds)
                    {
                        dmess("bounds " << get<0>(bounds) << " " << get<1>(bounds) << " " << get<2>(bounds) << " " << get<3>(bounds));
                    });
                }

                if(ImGui::MenuItem("Test 6"))
                {
                    std::function<void (geos::geom::Geometry *)> callback = [](geos::geom::Geometry * geom)
                    {
                        dmess("geom " << geom);
                    };

                    GeoClient::getInstance()->getGeometry(14, callback);
                }

                if(ImGui::MenuItem("Test 7"))
                {
                    doTest7 = true;
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
                if(ImGui::MenuItem("View Matrix"))
                {
                    showViewMatrixPanel = !showViewMatrixPanel;
                }
                if(ImGui::MenuItem("MVP Matrix"))
                {
                    showMVP_MatrixPanel = !showMVP_MatrixPanel;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    if(doIt)
    {
        dmess("doIt " << doIt);

        char buf[2048];

        sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                        "var dv = new DataView(buffer); \r\n"
                        "dv.setUint8(0,2); \r\n"
                        "dv.setUint32(1, Module.swap32(%i)); \r\n"
                        "//var view = new DataView(buffer, 1, 4); \r\n"
                        "//view.setUint32(12); \r\n"
                        "Module.connection.send(buffer); \r\n", 13);
                    
        emscripten_run_script(buf);
    }

    if(doTest7)
    {
        RenderiableCollection * layer = new RenderiableCollection();

        canvas->addRenderiable(layer);

        std::function<void (const size_t)> getNumGeoms = [layer](const size_t numGeoms)
        {
            GeoClient::getInstance()->getLayerBounds([numGeoms, layer](const AABB2D & bounds)
            {
                const mat4 trans = translate(   mat4(1.0),
                                                vec3((get<0>(bounds) + get<2>(bounds)) * -0.5,
                                                     (get<1>(bounds) + get<3>(bounds)) * -0.5,
                                                     0.0));

                std::function<void (geos::geom::Geometry *)> getGeom = [trans, layer](geos::geom::Geometry * geom)
                {
                    Renderiable * r = Renderiable::create(geom, trans);

                    r->setFillColor(vec4(0.3,0.0,0.3,1));
                        
                    r->setOutlineColor(vec4(1,0,0,1));

                    //canvas->addRenderiable(r);
                    layer->addRenderiable(r);
                };

                for(size_t i = 0; i < numGeoms; ++i)
                {
                    GeoClient::getInstance()->getGeometry(i, getGeom);
                }
            });
        };

        GeoClient::getInstance()->getNumGeoms(getNumGeoms);
    }

    // Rendering
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    canvas->render();

    static float time = 0.f;
    //if (!paused)
    {
        time += ImGui::GetIO().DeltaTime;
    }

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    /*
    const mat4 view = camera->getMatrix();
    const mat4 model = mat4(1.0);
    const mat4 projection = perspective(45.0, double(screenWidth) / double(screenHeight), 0.1, 100.0);
    const mat4 MVP = projection * view * model;
    */

    if(showViewMatrixPanel)
    {
        ImGui::Begin("View Matrix", &showViewMatrixPanel);
        //ImGui::Text(mat4ToStr(view).c_str());
        ImGui::End();
    }
    
    if(isFirst) { ImGui::SetNextWindowPos(ImVec2(0,0)) ;}

    if(showMVP_MatrixPanel)
    {
        ImGui::Begin("MVP Matrix", &showMVP_MatrixPanel);
        //ImGui::Text(mat4ToStr(MVP).c_str());
        ImGui::End();
    }

    isFirst = false;

    ImGui::Begin("Scene Window");

    //dmess("ImGui::IsWindowFocused() " << ImGui::IsWindowFocused());

    ImVec2 pos = ImGui::GetCursorScreenPos();

    auxCanvas->setArea(__(pos), __(sceneWindowSize));

    auxCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

    ImGui::GetWindowDrawList()->AddImage(   (void *)auxCanvas->render(),
                                            pos,
                                            ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                            ImVec2(0, 1),
                                            ImVec2(1, 0));
        
    sceneWindowSize = ImGui::GetWindowSize();

    ImGui::End();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();

#ifndef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);

    //refresh(window);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return;
    }

    auxCanvas->onMouseButton(window, button, action, mods);

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

    auxCanvas->onMousePosition(window, Vec2d(xpos, ypos));

    canvas->onMousePosition(window, Vec2d(xpos, ypos));

    //trackBallInteractor.setClickPoint(xpos, ypos);

    //trackBallInteractor.update();

    //if (render_when_mouse_up || mouse_buttons_down)
    {
        refresh(window);
    }
}

void scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    //dmess("ScrollCallback " << xoffset << " " << yoffset);

    auxCanvas->onMouseScroll(window, Vec2d(xoffset, yoffset));

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
    auxCanvas->onKey(window, key, scancode, action, mods);

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
    auxCanvas->onChar(window, c);

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
    dmess("width " << width << " height " << height);

    //trackBallInteractor.setScreenSize(width, height);

    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

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
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);

    /*
    trackBallInteractor.setScreenSize(width, height);

    camera = trackBallInteractor.getCamera();
    trackBallInteractor.getCamera()->reset();
    trackBallInteractor.setSpeed(3);
    */

    canvas = new Canvas(false);

    canvas->setArea(Vec2i(0,0), Vec2i(width, height));

    auxCanvas = new Canvas();
}

void initGeometry()
{
    Geometry * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Geometry * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    Geometry * ppp = scopedGeosGeometry(GeosUtil::makeBox(-0.05,-0.6,0.05,0.6));

    Geometry * pppp = scopedGeosGeometry(GeosUtil::makeBox(-0.6,-0.05,0.6,0.05));

    /*
    p = scopedGeosGeometry(p->buffer(0.1));

    p = scopedGeosGeometry(p->difference(pp));

    p = scopedGeosGeometry(p->difference(ppp));

    p = scopedGeosGeometry(p->difference(pppp));
    */

    const mat4 trans = scale(mat4(1.0), vec3(0.1, 0.1, 0.1));

    dmess("trans " << mat4ToStr(trans));

    Renderiable * r = Renderiable::create(p, trans);
    //Renderiable * r = Renderiable::create(p);

    r->setFillColor(vec4(0.3,0.3,0,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    auxCanvas->addRenderiable(r);
    canvas->addRenderiable(r);

    /*
    p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,-4,0.4));

    //r = Renderiable::create(p, trans);
    r = Renderiable::create(p);

    r->setFillColor(vec4(0.3,0.0,0.3,1));
        
    r->setOutlineColor(vec4(1,0,0,1));

    canvas->addRenderiable(r);
    //*/
}
