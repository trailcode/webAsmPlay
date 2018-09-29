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
#include <webAsmPlay/debug.h>
#include <webAsmPlay/GeosRenderiable.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tceGeom/vec2.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/globjects.h>
#include <globjects/NamedString.h>
#include <globjects/base/AbstractStringSource.h>
#include <globjects/NamedString.h>
#include <globjects/Shader.h>
#include <globjects/logging.h>
#include <globjects/base/File.h>

//using namespace gl;
using namespace std;
using namespace geos::geom;
using namespace rsmz;
using namespace glm;
using namespace globjects;
using namespace tce::geom;

#define ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

namespace
{
const GeometryFactory * geomFact;

extern "C" {
geos::geom::Polygon *MakeBox(double xmin, double ymin, double xmax, double ymax) {
    //cout << "boxa " << endl;

    geomFact = GeometryFactory::getDefaultInstance();
    //cout << "geomFact->getCoordinateSequenceFactory() " << geomFact->getCoordinateSequenceFactory() << endl;
    geos::geom::CoordinateSequence *temp = geomFact->getCoordinateSequenceFactory()->create((std::size_t) 0, 0);
    //cout << "temp " << temp << endl;

    temp->add(geos::geom::Coordinate(xmin, ymin));
    temp->add(geos::geom::Coordinate(xmin, ymax));
    temp->add(geos::geom::Coordinate(xmax, ymax));
    temp->add(geos::geom::Coordinate(xmax, ymin));
    //Must close the linear ring or we will get an error:
    //"Points of LinearRing do not form a closed linestring"
    temp->add(geos::geom::Coordinate(xmin, ymin));

    geos::geom::LinearRing *shell = geomFact->createLinearRing(temp);

    //NULL in this case could instead be a collection of one or more holes
    //in the interior of the polygon
    return geomFact->createPolygon(shell, NULL);
}
}
}

// Window dimensions
//const GLuint WIDTH = 800, HEIGHT = 600;

GLuint vao;
GLuint shaderProgram2;
GLint posAttrib;
GLint MVP_Attrib;

void setupAnotherShader()
{
    // Create Vertex Array Object
    dmess("setupAnotherShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 position;
        uniform mat4 MVP;
        void main()
        {
            gl_Position = MVP * vec4(position.xy, 0, 1);
        }
    )glsl";
    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        
        void main()
        {
            outColor = vec4(1,1,1,0.5);
        }
    )glsl";

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    dmess("vao " << vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    dmess("vbo " << vbo);

    GLfloat vertices[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);
    dmess("ebo " << ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    GLchar infoLog[512];

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint success = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    dmess("success " << success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
        return;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    dmess("success " << success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
        return;
    }

    // Link the vertex and fragment shader into a shader program
    shaderProgram2 = glCreateProgram();
    dmess("shaderProgram2 " << shaderProgram2);
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader);
    //glBindFragDataLocation(shaderProgram2, 0, "outColor");
    glLinkProgram(shaderProgram2);
    glUseProgram(shaderProgram2);

    // Specify the layout of the vertex data
    posAttrib = glGetAttribLocation(shaderProgram2, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    MVP_Attrib = glGetUniformLocation(shaderProgram2, "MVP");

    dmess("MVP_Attrib " << MVP_Attrib);

    /*
    GLint colAttrib = glGetAttribLocation(shaderProgram2, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    */
}

    static GLuint VBO, VAO, EBO;
    static GLuint shaderProgram;
    static GLFWwindow* window;
    static GLint resolutionLoc;
    static GLint timeLoc;
    static GLint vertexCountLoc;

void cleanup()
{
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
}

void Refresh(GLFWwindow* window)
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();

    glfwMarkWindowForRefresh(window);
#endif
    //cout << "Here Comment back!" << endl;
}

static bool show_test_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImColor(114, 144, 154);
static bool paused = false;
static bool render_when_mouse_up = false;
static int mouse_buttons_down = 0;

static bool mouse_buttons[GLFW_MOUSE_BUTTON_LAST + 1] = { false, };

extern "C" {
extern void my_js();
}

//extern "C" void invoke_callback(int callback_id, const char* text);

void my_c_function(int callback_id) {
    //invoke_callback( callback_id, "Hello World!" );
}

//====================

string mat4ToStr(const mat4 & m)
    {
       	char buf[1024];

        sprintf(buf,
                "[[% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
                " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
                " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
                " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]]\n",
                static_cast<double>(m[0][0]), static_cast<double>(m[0][1]), static_cast<double>(m[0][2]), static_cast<double>(m[0][3]),
                static_cast<double>(m[1][0]), static_cast<double>(m[1][1]), static_cast<double>(m[1][2]), static_cast<double>(m[1][3]),
                static_cast<double>(m[2][0]), static_cast<double>(m[2][1]), static_cast<double>(m[2][2]), static_cast<double>(m[2][3]),
                static_cast<double>(m[3][0]), static_cast<double>(m[3][1]), static_cast<double>(m[3][2]), static_cast<double>(m[3][3]));

        return buf;
    }

TrackBallInteractor trackBallInteractor;
Camera * camera = NULL;

void mainLoop(GLFWwindow* window) {
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
            ImGui::EndMainMenuBar();
        }
    }

    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    static float time = 0.f;
    if (!paused)
    {
        time += ImGui::GetIO().DeltaTime;
    }

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    mat4 view = camera->getMatrix();

    ImGui::Begin("View Matrix");
    ImGui::Text(mat4ToStr(view).c_str());
    ImGui::End();
    //dmess("view " << mat4ToStr(view));
    mat4 model = mat4(1.0);
    mat4 projection = perspective(45.0, double(display_w) / double(display_h), 0.1, 100.0);

    mat4 MVP = projection * view * model;

    /*
    //glLinkProgram(shaderProgram2);
    glUseProgram(shaderProgram2);

    glUniformMatrix4fv(MVP_Attrib, 1, false, glm::value_ptr(MVP));

    glBindVertexArray(vao);
    // Specify the layout of the vertex data
    //glEnableVertexAttribArray(posAttrib);
    //glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    */

    // =========================

    vector<Geometry *> toDelete;

    //*
    Polygon * pp = MakeBox(-0.1,-0.1,0.1,0.1);
    toDelete.push_back(pp);
    Polygon * p = MakeBox(-0.5,-0.5,0.5,0.5);
    toDelete.push_back(p);

    p = dynamic_cast<Polygon *>(p->buffer(0.1));
    toDelete.push_back(p);
    p = dynamic_cast<Polygon *>(p->difference(pp));
    toDelete.push_back(p);

    const LineString * ring = p->getExteriorRing();

    const size_t numPoints = ring->getNumPoints();

    //cout << "ring  " << ring << " numPoints " << numPoints << " ring->isClosed(); " << ring->isClosed() << endl;

    //cout << " p " << p->getArea() << " num Points " << ring->getNumPoints() << endl;

    glDisable(GL_DEPTH_TEST);

    for(size_t i = 0; i < numPoints; ++i)
    {
        Point * p = ring->getPointN(i);

        //cout << " p.x " << p->getX() << " p.y " << p->getY() << endl;
    }

    unique_ptr<GeosRenderiable> r(GeosRenderiable::create(p));

    if(r)
    {
        r->setFillColor(vec4(1,1,0,0.2));
        
        r->render(MVP);
    }

    for(Geometry * g : toDelete)
    {
        geomFact->destroyGeometry(g);
    }
    //*/

    ImGui::Render();

#ifndef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);

    if (!paused)
    {
        Refresh(window);
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
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
    
    Refresh(window);
}

Vec2i lastShiftKeyDownMousePos;

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    //dmess("x " << xpos << " y " << ypos);

    trackBallInteractor.setClickPoint(xpos, ypos);
    trackBallInteractor.update();

    //if (render_when_mouse_up || mouse_buttons_down)
    {
        Refresh(window);
    }
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
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
    Refresh(window);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

    Refresh(window);
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfwGL3_CharCallback(window, c);
#else
    ImGui_ImplGlfw_CharCallback(window, c);
#endif
    Refresh(window);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    //dmess("width " << width << " height " << height);

    trackBallInteractor.setScreenSize(width, height);

    Refresh(window);
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        Refresh(window);
    }
}

void CursorEnterCallback(GLFWwindow* window, int /* entered */)
{
    Refresh(window);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void initOpenGL(GLFWwindow* window)
{
    // Define the viewport dimensions
    static int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);

    trackBallInteractor.setScreenSize(width, height);

    //dmess("width " << width << " height " << height);

    camera = trackBallInteractor.getCamera();
    trackBallInteractor.getCamera()->reset();
    trackBallInteractor.setSpeed(3);

    setupAnotherShader();

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}
