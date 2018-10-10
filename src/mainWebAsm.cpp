#include <istream>
#include <streambuf>
#include <geos.h>
#include <emscripten/bind.h>
#include <geoServer/GeoServerBase.h>
#include <webAsmPlay/Debug.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

using namespace std;
using namespace geos::io;
using namespace geos::geom;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

void errorCallback(int error, const char* description);

void mouseButtonCallback    (GLFWwindow* window, int button, int action, int mods);
void scrollCallback         (GLFWwindow* window, double xoffset, double yoffset);
void keyCallback            (GLFWwindow* window, int key, int scancode, int action, int mods);
void charCallback           (GLFWwindow* window, unsigned int c);
void windowFocusCallback    (GLFWwindow* window, int focused);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback      (GLFWwindow* window, double xpos, double ypos);
void cursorEnterCallback    (GLFWwindow* window, int entered);
void initOpenGL             (GLFWwindow* window);
void initGeometry           ();
void mainLoop               (GLFWwindow* window);
void refresh                (GLFWwindow* window);

// The MAIN function, from here we start the application and run the game loop
int main()
{
    glfwSetErrorCallback(errorCallback);

    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // APPLE
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //glfwWindowHint(GLFW_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "WebAsmPlay", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, keyCallback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Setup ImGui binding
    bool install_callbacks = false;
    ImGui_ImplGlfwGL3_Init(window, install_callbacks);

    glfwSetMouseButtonCallback      (window, mouseButtonCallback);
    glfwSetScrollCallback           (window, scrollCallback);
    glfwSetKeyCallback              (window, keyCallback);
    glfwSetCharCallback             (window, charCallback);
    glfwSetWindowFocusCallback      (window, windowFocusCallback);
    glfwSetFramebufferSizeCallback  (window, framebufferSizeCallback);
    glfwSetCursorPosCallback        (window, cursorPosCallback);
    glfwSetCursorEnterCallback      (window, cursorEnterCallback);


    // Setup style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    initOpenGL(window);

    initGeometry();

    glfwSetWindowRefreshCallback(window, mainLoop);

    refresh(window);

    return 0;
}
