#include <cmath>
#include <imgui.h>

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
    #define IMGUI_API
    #include <imgui_impl_glfw_gl3.h>
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
#include <webAsmPlay/GeosRenderiable.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tceGeom/vec2.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/FrameBuffer.h>
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

TrackBallInteractor trackBallInteractor;
Camera * camera = NULL;

bool showViewMatrixPanel = true;
bool showMVP_MatrixPanel = true;

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

                    //emscripten_run_script("fileInput.click();");
                    //emscripten_run_script("alert(fileSelector)");
                    //emscripten_run_script("Module.my_js();");

                    //my_c_function(0);

                    //EM_ASM("Module.my_js();");

                    //my_js();
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

    // Rendering
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    static float time = 0.f;
    //if (!paused)
    {
        time += ImGui::GetIO().DeltaTime;
    }

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    const mat4 view = camera->getMatrix();
    const mat4 model = mat4(1.0);
    const mat4 projection = perspective(45.0, double(screenWidth) / double(screenHeight), 0.1, 100.0);
    const mat4 MVP = projection * view * model;

    if(showViewMatrixPanel)
    {
        ImGui::Begin("View Matrix", &showViewMatrixPanel);
        ImGui::Text(mat4ToStr(view).c_str());
        ImGui::End();
    }
    
    if(isFirst) { ImGui::SetNextWindowPos(ImVec2(0,0)) ;}

    if(showMVP_MatrixPanel)
    {
        ImGui::Begin("MVP Matrix", &showMVP_MatrixPanel);
        ImGui::Text(mat4ToStr(MVP).c_str());
        ImGui::End();
    }

    isFirst = false;

    Polygon * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Polygon * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    p = scopedGeosGeometry(dynamic_cast<Polygon *>(p->buffer(0.1)));

    p = scopedGeosGeometry(dynamic_cast<Polygon *>(p->difference(pp)));

    unique_ptr<GeosRenderiable> r(GeosRenderiable::create(p));

    // Render to our framebuffer
    //glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);
    //glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    /*
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0,0,640,480);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    */

    frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, Vec2i(sceneWindowSize.x, sceneWindowSize.y));

    frameBuffer->bind();

    glViewport(0,0,sceneWindowSize.x,sceneWindowSize.y);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    if(r)
    {
        r->setFillColor(vec4(1,1,0,1));
        
        r->render(MVP);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Begin("Scene Window");

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::GetWindowDrawList()->AddImage(
        (void *)frameBuffer->getTextureID(),
        pos,
        ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y), ImVec2(0, 1), ImVec2(1, 0));

    sceneWindowSize = ImGui::GetWindowSize();

    ImGui::End();

    ImGui::Render();

#ifndef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);

    //refresh(window);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return;
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

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    //dmess("x " << xpos << " y " << ypos);

    trackBallInteractor.setClickPoint(xpos, ypos);
    trackBallInteractor.update();

    //if (render_when_mouse_up || mouse_buttons_down)
    {
        refresh(window);
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    //dmess("ScrollCallback " << xoffset << " " << yoffset);

    ImGuiContext & g = *GImGui;

    if(!g.IO.WantCaptureMouse)
    {
        int state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        
        if (state == GLFW_PRESS)
        {
            //dmess("GLFW_KEY_LEFT_SHIFT");
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            lastShiftKeyDownMousePos += Vec2d(xoffset, 0);
            trackBallInteractor.setClickPoint(lastShiftKeyDownMousePos.x % display_w, lastShiftKeyDownMousePos.y % display_h);
            trackBallInteractor.update();
            //dmess("lastShiftKeyDownMousePos " << lastShiftKeyDownMousePos);
        }

        state = glfwGetKey(window, GLFW_KEY_LEFT_ALT);

        if (state == GLFW_PRESS)
        {
            dmess("GLFW_KEY_LEFT_ALT");
        }

        const double delta = yoffset;

        trackBallInteractor.setScrollDirection(delta > 0);
        trackBallInteractor.update();
    }

#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
#else
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    refresh(window);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //dmess("key " << key << " scancode " << scancode << " action " << action << " mods " << mods);
    switch(key)
    {
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_LEFT_ALT:

            trackBallInteractor.setLeftClicked(action);

            if(action)
            {
                double xPos;
                double yPos;
                glfwGetCursorPos(window, &xPos, &yPos);
                lastShiftKeyDownMousePos = Vec2i(xPos, yPos);
            }

            break;
    }

    switch(key)
    {
        case GLFW_KEY_LEFT_SHIFT: trackBallInteractor.setMotionLeftClick(ARC); break;
        case GLFW_KEY_LEFT_ALT:   trackBallInteractor.setMotionLeftClick(PAN); break;
    }
     
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
#else
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif

    refresh(window);
}

void charCallback(GLFWwindow* window, unsigned int c)
{
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_CharCallback(window, c);
#else
    ImGui_ImplGlfw_CharCallback(window, c);
#endif
    refresh(window);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    //dmess("width " << width << " height " << height);

    trackBallInteractor.setScreenSize(width, height);

    refresh(window);
}

void windowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        refresh(window);
    }
}

void cursorEnterCallback(GLFWwindow* window, int /* entered */)
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

    trackBallInteractor.setScreenSize(width, height);

    camera = trackBallInteractor.getCamera();
    trackBallInteractor.getCamera()->reset();
    trackBallInteractor.setSpeed(3);

    //glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    /*
    glGenFramebuffers(1, &framebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);

    // The texture we're going to render to
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // The depth buffer
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { dmess("Error!") ;}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //*/

    /*
    int SCR_WIDTH = 640;
    int SCR_HEIGHT = 480;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
}
