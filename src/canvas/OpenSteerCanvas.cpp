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

#include <memory>
#include <glm/gtx/transform.hpp>
#include <OpenSteer/OpenSteerDemo.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/canvas/OpenSteerCanvas.h>

using namespace std;
using namespace glm;

OpenSteerCanvas::OpenSteerCanvas(const bool   useFrameBuffer,
                                 const vec4 & clearColor) : Canvas(useFrameBuffer, clearColor)
{

}

OpenSteerCanvas::~OpenSteerCanvas()
{

}

void openSteerDisplayFunc();

GLuint OpenSteerCanvas::render()
{
    const vec3 scale(0.1, 0.1, 0.1);

    const vec3 lookat = __(OpenSteer::OpenSteerDemo::camera.target)     * scale;
    const vec3 pos    = __(OpenSteer::OpenSteerDemo::camera.position()) * scale;
    const vec3 up     = __(OpenSteer::OpenSteerDemo::camera.up());

    const mat4 rotate = glm::rotate(radians(90.0f), vec3(1, 0, 0));

    getCamera()->setCenter(lookat);
    getCamera()->setEye   (pos);
    getCamera()->setUp    (up);

    getCamera()->update();

    if(!preRender()) { return 0 ;}

    /*
    openSteerDisplayFunc();

    unique_ptr<Renderable>(DeferredRenderable::createFromQueued())->render(this);
    //*/

    return postRender();
}

extern float g_openSteerWindowHeight;
extern float g_openSteerWindowWidth;

ivec2 OpenSteerCanvas::setFrameBufferSize(const ivec2 & fbSize, const ivec2 & upperLeft)
{
    g_openSteerWindowWidth  = (float)fbSize.x;
    g_openSteerWindowHeight = (float)fbSize.y;

    return Canvas::setFrameBufferSize(fbSize, upperLeft);
}

void openSteerKeyboardFunc (unsigned char key);

void OpenSteerCanvas::onChar(GLFWwindow * window, const size_t c)
{
    dmess("OpenSteerCanvas::onChar " << (char)c);

    //openSteerKeyboardFunc((char)c);
}

