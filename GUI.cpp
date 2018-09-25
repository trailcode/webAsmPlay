//OUT#include <iostream>
#include <cmath>

//#include <emscripten.h>

// GLEW
//#define GLEW_STATIC
//#include <GL/glew.h>

#include <GL/gl3w.h>    // Initialize with gl3wInit()

// GLFW
#include <GLFW/glfw3.h>

#include <imgui.h>
//#include "imgui_impl_glfw_gl3.h"

#include "imgui_impl_opengl3.h"

#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
//#include <imguidock.h>
#include <iostream>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include <GEOS/geom/LineString.h>
#include <GEOS/geom/Point.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include "../TrackBallInteractor.h"
#include "../Camera.h"
#include "../debug.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tceGeom/vec2.h>
//#include <GLU/tessellate.h>

using namespace std;
using namespace geos::geom;
using namespace rsmz;
using namespace glm;
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
const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar* vertexShaderSource = R"(#version 330

uniform float time;
uniform vec2 resolution;
uniform float vertexCount;

out vec4 v_color;

#define PI radians(180.)

vec3 hsv2rgb(vec3 c) {
  c = vec3(c.x, clamp(c.yz, 0.0, 1.0));
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

mat4 rotX(float angleInRadians) {
    float s = sin(angleInRadians);
    float c = cos(angleInRadians);

    return mat4(
      1, 0, 0, 0,
      0, c, s, 0,
      0, -s, c, 0,
      0, 0, 0, 1);
}

mat4 rotY(float angleInRadians) {
    float s = sin(angleInRadians);
    float c = cos(angleInRadians);

    return mat4(
      c, 0,-s, 0,
      0, 1, 0, 0,
      s, 0, c, 0,
      0, 0, 0, 1);
}

mat4 rotZ(float angleInRadians) {
    float s = sin(angleInRadians);
    float c = cos(angleInRadians);

    return mat4(
      c,-s, 0, 0,
      s, c, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1);
}

mat4 trans(vec3 trans) {
  return mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    trans, 1);
}

mat4 ident() {
  return mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);
}

mat4 scale(vec3 s) {
  return mat4(
    s[0], 0, 0, 0,
    0, s[1], 0, 0,
    0, 0, s[2], 0,
    0, 0, 0, 1);
}

mat4 uniformScale(float s) {
  return mat4(
    s, 0, 0, 0,
    0, s, 0, 0,
    0, 0, s, 0,
    0, 0, 0, 1);
}

mat4 persp(float fov, float aspect, float zNear, float zFar) {
  float f = tan(PI * 0.5 - 0.5 * fov);
  float rangeInv = 1.0 / (zNear - zFar);

  return mat4(
    f / aspect, 0, 0, 0,
    0, f, 0, 0,
    0, 0, (zNear + zFar) * rangeInv, -1,
    0, 0, zNear * zFar * rangeInv * 2., 0);
}

mat4 trInv(mat4 m) {
  mat3 i = mat3(
    m[0][0], m[1][0], m[2][0],
    m[0][1], m[1][1], m[2][1],
    m[0][2], m[1][2], m[2][2]);
  vec3 t = -i * m[3].xyz;

  return mat4(
    i[0], t[0],
    i[1], t[1],
    i[2], t[2],
    0, 0, 0, 1);
}

mat4 lookAt(vec3 eye, vec3 target, vec3 up) {
  vec3 zAxis = normalize(eye - target);
  vec3 xAxis = normalize(cross(up, zAxis));
  vec3 yAxis = cross(zAxis, xAxis);

  return mat4(
    xAxis, 0,
    yAxis, 0,
    zAxis, 0,
    eye, 1);
}

mat4 cameraLookAt(vec3 eye, vec3 target, vec3 up) {
  return inverse(lookAt(eye, target, up));
}

// hash function from https://www.shadertoy.com/view/4djSRW
float hash(float p) {
    vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
    p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
    return fract(p2.x * p2.y * 95.4337);
}

// times 2 minus 1
float t2m1(float v) {
  return v * 2. - 1.;
}

// times .5 plus .5
float t5p5(float v) {
  return v * 0.5 + 0.5;
}

float inv(float v) {
  return 1. - v;
}

#define CUBE_POINTS_PER_FACE 6.
#define FACES_PER_CUBE 6.
#define POINTS_PER_CUBE (CUBE_POINTS_PER_FACE * FACES_PER_CUBE)
void getCubePoint(const float id, out vec3 position, out vec3 normal) {
  float quadId = floor(mod(id, POINTS_PER_CUBE) / CUBE_POINTS_PER_FACE);
  float sideId = mod(quadId, 3.);
  float flip   = mix(1., -1., step(2.5, quadId));
  // 0 1 2  1 2 3
  float facePointId = mod(id, CUBE_POINTS_PER_FACE);
  float pointId = mod(facePointId - floor(facePointId / 3.0), 6.0);
  float a = pointId * PI * 2. / 4. + PI * 0.25;
  vec3 p = vec3(cos(a), 0.707106781, sin(a)) * flip;
  vec3 n = vec3(0, 1, 0) * flip;
  float lr = mod(sideId, 2.);
  float ud = step(2., sideId);
  mat4 mat = rotX(lr * PI * 0.5);
  mat *= rotZ(ud * PI * 0.5);
  position = (mat * vec4(p, 1)).xyz;
  normal = (mat * vec4(n, 0)).xyz;
}

void main() {
  float pointId = float(gl_VertexID);

  vec3 pos;
  vec3 normal;
  getCubePoint(pointId, pos, normal);
  float cubeId = floor(pointId / 36.);
  float numCubes = floor(vertexCount / 36.);
  float down = floor(sqrt(numCubes));
  float across = floor(numCubes / down);

  float cx = mod(cubeId, across);
  float cy = floor(cubeId / across);

  float cu = cx / (across - 1.);
  float cv = cy / (down - 1.);

  float ca = cu * 2. - 1.;
  float cd = cv * 2. - 1.;

  float tm = time * 0.1;
  mat4 mat = persp(radians(60.0), resolution.x / resolution.y, 0.1, 1000.0);
  vec3 eye = vec3(cos(tm) * 1., sin(tm * 0.9) * .1 + 1., sin(tm) * 1.);
  vec3 target = vec3(0);
  vec3 up = vec3(0,1,0);

  mat *= cameraLookAt(eye, target, up);
  mat *= trans(vec3(ca, 0, cd) * 2.);
  mat *= rotX(time + abs(ca) * 5.);
  mat *= rotZ(time + abs(cd) * 6.);
  mat *= uniformScale(0.03);


  gl_Position = mat * vec4(pos, 1);
  vec3 n = normalize((mat * vec4(normal, 0)).xyz);

  vec3 lightDir = normalize(vec3(0.3, 0.4, -1));

  float hue = abs(ca * cd) * 2.;
  float sat = mix(1., 0., abs(ca));
  float val = mix(1., 0.5, abs(cd));
  vec3 color = hsv2rgb(vec3(hue, sat, val));
  v_color = vec4(color * (dot(n, lightDir) * 0.5 + 0.5), 1);
}
)";

const GLchar* fragmentShaderSource = R"(#version 330
in vec4 v_color;
out vec4 color;
void main()
{
   color = vec4(v_color.x, v_color.y, v_color.z, 0.2);
}
)";

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

GLuint vao;
GLuint shaderProgram2;
GLint posAttrib;
GLint MVP_Attrib;

void setupAnotherShader()
{
    // Create Vertex Array Object
    dmess("setupAnotherShader");

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

void cleanup() {
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    //OUT std::cout << "exit main" << std::endl;
}

void Refresh(GLFWwindow* window)
{
    glfwPollEvents();
    //glfwMarkWindowForRefresh(window);
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

    //cout << "main loop" << endl;
    //glfwWaitEvents();

    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    //glfwPollEvents();


    dmess("Put back");
    //ImGui_ImplGlfwGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();

    // Render
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    /*
    {
        static float f = 0.0f;
        static float frameTimes[100] = {0.f};
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
        memcpy(&frameTimes[0], &frameTimes[1], sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[ARRAYSIZE(frameTimes) - 1] = ImGui::GetIO().Framerate;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::PlotLines("Frame History", frameTimes, ARRAYSIZE(frameTimes), 0, "", 0.0f, 100.0f, ImVec2(0, 50));
        ImGui::Checkbox("Pause", &paused);
        ImGui::Checkbox("Render When Mouse Up", &render_when_mouse_up);
    }
     //*/

    {
        /*
        if (ImGui::Begin("imguidock window (= lumix engine's dock system)",NULL,ImVec2(500, 500),0.95f,ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::BeginDockspace();
            static char tmp[128];
            for (int i=0;i<10;i++)  {
                sprintf(tmp,"Dock %d",i);
                if (i==9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
                if(ImGui::BeginDock(tmp))  {
                    ImGui::Text("Content of dock window %d goes here",i);
                }
                ImGui::EndDock();
            }
            ImGui::EndDockspace();
        }
        //*/

        ImGui::Begin("Style Editoraaa");

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

        ImGui::BulletText("Double-click on title bar to collapse window.");
        ImGui::BulletText("Click and drag on lower right corner to resize window.");
        ImGui::BulletText("Click and drag on any empty space to move window.");
        ImGui::BulletText("Mouse Wheel to scroll.");
        if (ImGui::GetIO().FontAllowUserScaling)
            ImGui::BulletText("CTRL+Mouse Wheel to zoom window contents.");
        ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
        ImGui::BulletText("CTRL+Click on a slider or drag box to input text.");
        ImGui::BulletText(
                "While editing text:\n"
                "- Hold SHIFT or use mouse to select text\n"
                "- CTRL+Left/Right to word jump\n"
                "- CTRL+A or double-click to select all\n"
                "- CTRL+X,CTRL+C,CTRL+V clipboard\n"
                "- CTRL+Z,CTRL+Y undo/redo\n"
                "- ESCAPE to revert\n"
                "- You can apply arithmetic operators +,*,/ on numerical values.\n"
                "  Use +- to subtract.\n");
        ImGui::End();
    }

    /*
    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }
    */

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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glUniform1f(timeLoc, time);
    int numVerts = 100000;
    glUniform1f(vertexCountLoc, numVerts);
    glUniform2f(resolutionLoc, display_w, display_h);
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);

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

    //glLinkProgram(shaderProgram2);
    glUseProgram(shaderProgram2);

    glUniformMatrix4fv(MVP_Attrib, 1, false, glm::value_ptr(MVP));

    glBindVertexArray(vao);
    // Specify the layout of the vertex data
    //glEnableVertexAttribArray(posAttrib);
    //glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // =========================

    //*
    Polygon * p = MakeBox(-1,-1,1,1);

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

    geomFact->destroyGeometry(p);

    //*/

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

    //ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    dmess("Put bak");
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

    //ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    dmess("Put back");
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
    
    //ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    dmess("Put back");
    Refresh(window);
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
    //ImGui_ImplGlfwGL3_CharCallback(window, c);
    dmess("Put back");
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

    dmess("width " << width << " height " << height);

    camera = trackBallInteractor.getCamera();
    trackBallInteractor.getCamera()->reset();
    trackBallInteractor.setSpeed(3);

    setupAnotherShader();

    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
        return;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
        return;
    }
    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::PROGRAM::LINK_FAILED: %s\n", infoLog);
        return;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    resolutionLoc = glGetUniformLocation(shaderProgram, "resolution");
    timeLoc = glGetUniformLocation(shaderProgram, "time");
    vertexCountLoc = glGetUniformLocation(shaderProgram, "vertexCount");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}
