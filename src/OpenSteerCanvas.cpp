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
#include <OpenSteer/OpenSteerDemo.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/DeferredRenderable.h>
#include <webAsmPlay/OpenSteerCanvas.h>

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
    if(!preRender()) { return 0 ;}

    openSteerDisplayFunc();

    unique_ptr<Renderable>(DeferredRenderable::createFromQueued())->render(this);

    return postRender();
}

extern float openSteerWindowHeight;
extern float openSteerWindowWidth;

void OpenSteerCanvas::setArea(const ivec2 & upperLeft, const ivec2 & size)
{
    openSteerWindowWidth  = size.x;
    openSteerWindowHeight = size.y;

    if(size != this->size)
    {
        dmess("Resize!");
    }

    Canvas::setArea(upperLeft, size);
}