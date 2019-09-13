
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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/canvas/GeosTestCanvas.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

namespace
{
    ivec2 a_lastShiftKeyDownMousePos;
}

void GUI::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    for(auto c : s_auxCanvases) { c->onMouseButton(window, button, action, mods) ;}
    
    if(!GImGui->IO.WantCaptureMouse)
    {
        s_canvas->onMouseButton(window, button, action, mods);

        if(action == GLFW_PRESS)
        {
            switch(GUI::getMode())
            {
            case SET_PATH_START_POINT:

                // TODO code refactor!
                s_client->getNetwork()->setStartEdge(s_client->pickLineStringRenderable(s_canvas->getCursorPosWC()));

                break;

            case FIND_PATH:

                s_client->getNetwork()->findPath(s_client->pickLineStringRenderable(s_canvas->getCursorPosWC()));

                break;
            }
        }
    }

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    refresh();
}

void GUI::cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    for(auto c : s_auxCanvases)
	{
		const auto localPos = vec2(xpos, ypos) - vec2(c->getUpperLeft());

		c->onMousePosition(window, localPos);
	}

    s_canvas->onMousePosition(window, vec2(xpos, ypos));

    refresh();
}

void GUI::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    //dmess("scrollCallback " << xoffset << " " << yoffset);

    for(auto c : s_auxCanvases) { c->onMouseScroll(window, vec2(xoffset, yoffset)) ;}

    if(!GImGui->IO.WantCaptureMouse) { s_canvas->onMouseScroll(window, vec2(xoffset, yoffset)) ;}

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    refresh();
}

void GUI::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    for(auto c : s_auxCanvases) { c->onKey(window, key, scancode, action, mods) ;}

    if(!GImGui->IO.WantCaptureKeyboard) { s_canvas->onKey(window, key, scancode, action, mods) ;}
 
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    refresh();
}

void GUI::charCallback(GLFWwindow * window, unsigned int c)
{
    for(auto ac : s_auxCanvases) { ac->onChar(window, c) ;}

    s_canvas->onChar(window, c);

    ImGui_ImplGlfw_CharCallback(window, c);

    refresh();
}

void GUI::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    int fbWidth, fbHeight;

    glfwGetFramebufferSize(s_mainWindow, &fbWidth, &fbHeight); // TODO Perhaps original passed in? 

    s_canvas->setFrameBufferSize(ivec2(fbWidth, fbHeight));

    // Need to use this to get true size because of retina displays.
    glfwGetWindowSize(window, &width, &height);

    refresh();
}


void GUI::windowFocusCallback(GLFWwindow* window, int focused)
{
    if(focused) { refresh() ;}
}

void GUI::cursorEnterCallback(GLFWwindow * window, int /* entered */)
{
    refresh();
}

void GUI::setupCallbacks(GLFWwindow* window)
{
    s_mainWindow = window;

    glfwSetMouseButtonCallback      (window, mouseButtonCallback);
    glfwSetScrollCallback           (window, scrollCallback);
    glfwSetKeyCallback              (window, keyCallback);
    glfwSetCharCallback             (window, charCallback);
    glfwSetWindowFocusCallback      (window, windowFocusCallback);
    glfwSetFramebufferSizeCallback  (window, framebufferSizeCallback);
    glfwSetCursorPosCallback        (window, cursorPosCallback);
    glfwSetCursorEnterCallback      (window, cursorEnterCallback);
}
