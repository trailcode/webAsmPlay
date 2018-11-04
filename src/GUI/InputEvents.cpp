
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

#include <glm/vec2.hpp>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace glm;

namespace
{
  ivec2 lastShiftKeyDownMousePos;
}

void GUI::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    geosTestCanvas->onMouseButton(window, button, action, mods);

    if(!GImGui->IO.WantCaptureMouse) { canvas->onMouseButton(window, button, action, mods) ;}

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    refresh(window);
}

void GUI::cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    //dmess("x " << xpos << " y " << ypos);

    geosTestCanvas->onMousePosition(window, vec2(xpos, ypos));

    canvas->onMousePosition(window, vec2(xpos, ypos));

    refresh(window);
}

void GUI::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    //dmess("ScrollCallback " << xoffset << " " << yoffset);

    geosTestCanvas->onMouseScroll(window, vec2(xoffset, yoffset));

    if(!GImGui->IO.WantCaptureMouse) { canvas->onMouseScroll(window, vec2(xoffset, yoffset)) ;}

    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    refresh(window);
}

void GUI::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    geosTestCanvas->onKey(window, key, scancode, action, mods);

    if(!GImGui->IO.WantCaptureKeyboard) { canvas->onKey(window, key, scancode, action, mods) ;}
 
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    refresh(window);
}

void GUI::charCallback(GLFWwindow * window, unsigned int c)
{
    geosTestCanvas->onChar(window, c);

    canvas->onChar(window, c);

    ImGui_ImplGlfw_CharCallback(window, c);

    refresh(window);
}

void GUI::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Need to use this to get true size because of retina displays.
    glfwGetWindowSize(window, &width, &height);

    canvas->setArea(ivec2(0,0), ivec2(width, height));

    refresh(window);
}

void GUI::windowFocusCallback(GLFWwindow* window, int focused)
{
    if(focused) { refresh(window) ;}
}

void GUI::cursorEnterCallback(GLFWwindow * window, int /* entered */)
{
    refresh(window);
}

void GUI::setupCallbacks(GLFWwindow* window)
{
    glfwSetMouseButtonCallback      (window, mouseButtonCallback);
    glfwSetScrollCallback           (window, scrollCallback);
    glfwSetKeyCallback              (window, keyCallback);
    glfwSetCharCallback             (window, charCallback);
    glfwSetWindowFocusCallback      (window, windowFocusCallback);
    glfwSetFramebufferSizeCallback  (window, framebufferSizeCallback);
    glfwSetCursorPosCallback        (window, cursorPosCallback);
    glfwSetCursorEnterCallback      (window, cursorEnterCallback);
}