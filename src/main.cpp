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

#ifndef __EMSCRIPTEN__
    #include <curl/curl.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/Python.h>
#include <webAsmPlay/GUI/GUI.h>

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions 
#include <webAsmPlay/OpenGL_Util.h>

#include <webAsmPlay/bing/StreetSide.h>

#include <boost/python.hpp>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void errorCallback(int error, const char* description);

extern std::thread * openSteerThread;

using namespace boost::python;

void my_exePy(const std::string command)
{
	try {
      //Py_Initialize();
   
      object main_module((
      handle<>(borrowed(PyImport_AddModule("__main__")))));
  
    object main_namespace = main_module.attr("__dict__");
  
    handle<> ignored(( PyRun_String( command.c_str(),
                                     Py_file_input,
                                     main_namespace.ptr(),
                                     main_namespace.ptr() ) ));

	

  } catch( error_already_set ) {
    PyErr_Print();
  }
}

#include <webAsmPlay/Util.h> 

int main(int, char**)
{
	//exit(0);
	Python::initPython();

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 330";
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "WebAsmPlay", nullptr, nullptr);

	/*
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	dmess("mode->width " << mode->width << " mode->height " << mode->height);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "My Title", glfwGetPrimaryMonitor(), nullptr);
	//*/

	/*
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	const GLFWvidmode * mode = glfwGetVideoMode(monitors[0]);

	// switch to full screen
	//glfwSetWindowMonitor( GUI::getMainWindow(), glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 0 );
	glfwSetWindowMonitor( window, monitors[0], 0, 0, mode->width, mode->height, mode->refreshRate);
	*/

    //GLFWwindow* window = glfwCreateWindow(640, 480, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
	// Initialize OpenGL loader
	//gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	/*
	if (gl3wInit())
	{
		dmess("failed to initialize OpenGL\n");
		exit(-1);
	}
	*/

	if (glewInit())
	{
		dmess("failed to initialize OpenGL\n");

		exit(-1);
	}
	
    curl_global_init( CURL_GLOBAL_ALL );

#endif

    glfwSwapInterval(0); // Enable vsync

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

    //window->Viewport->DpiScale = 2.0;

	// Setup style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //return 0;

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    viewport->DpiScale = 1.0;
	
    GUI::setupCallbacks(window);
    GUI::initOpenGL();
	GUI::initBubbleFacePanels();
	GUI::initRenderSettingsPanel();
    GUI::loadState();
    GUI::createWorld();

	//auto testa = readFile("C:/src/test1.py");
	auto testa = readFile("C:/src/test.py");

	//dmess("Here " << Python::execute(testa));
	
	//dmess(Python::execute(readFile("objectDetection.py")));

    #ifdef __EMSCRIPTEN__
        
        glfwSetWindowRefreshCallback(window, GUI::mainLoop);

        GUI::refresh();

    #else

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            if (glfwWindowShouldClose(window)) { break ;}
            
            glfwPollEvents();
            //glfwWaitEvents();
			
			GUI::doQueue();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            GUI::mainLoop(window);
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
        
    #endif

    GUI::shutdown();

    return 0;
}
