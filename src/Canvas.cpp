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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/RenderablePoint.h>
#include <webAsmPlay/SkyBox.h>
#include <webAsmPlay/Canvas.h>

using namespace std;
using namespace rsmz;
using namespace glm;
using namespace geos::geom;

#ifdef __EMSCRIPTEN__
    using namespace emscripten;
#endif

namespace
{
    std::vector<Canvas *> instances;
}

Canvas::Canvas( const bool   useFrameBuffer,
                const vec4 & clearColor) : trackBallInteractor (NULL),
                                            frameBuffer        (NULL),
                                            wantMouseCapture   (true),
                                            useFrameBuffer     (useFrameBuffer),
                                            clearColor         (clearColor),
                                            skyBox             (NULL),
                                            enabled            (true)
{
    trackBallInteractor = new TrackBallInteractor();

    instances.push_back(this);

    //cursor = RenderablePoint::create(vec3(0,0,0));
    cursor = NULL;
}

Canvas::~Canvas()
{
    delete trackBallInteractor;
    delete frameBuffer;

    // TODO remove from instances!
}

void Canvas::setArea(const ivec2 & upperLeft, const ivec2 & size)
{
    this->upperLeft = upperLeft;
    this->size      = size;

    if(useFrameBuffer) { frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, size) ;}

    trackBallInteractor->setScreenSize(size.x, size.y);
}

bool Canvas::preRender()
{
    if(!enabled) { return false ;}

    if(!size.x || !size.y)
    {
        dmess("Error canvas size is not valid!");

        return false;
    }

    Camera * camera = trackBallInteractor->getCamera();

    view         = camera->getMatrix();
    model        = mat4(1.0);
    projection   = perspective(45.0, double(size.x) / double(size.y), 0.01, 300.0);
    MV           = view * model;
    MVP          = projection * MV;

    if(useFrameBuffer)
    {
        frameBuffer->bind();
    
        GL_CHECK(glViewport(0, 0, size.x, size.y));
    }

    //GL_CHECK(glActiveTexture(GL_TEXTURE0));

    //GL_CHECK(glDisable(GL_TEXTURE_2D));

    if(skyBox) { skyBox->render(getCamera()->getMatrix(), getProjectionRef()) ;}

    else
    {
        GL_CHECK(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
    }

    return true;
}

GLuint Canvas::render()
{
    if(!preRender()) { return 0 ;}

    lock_guard<mutex> _(renderiablesMutex);

    for(Renderable * r : renderiables) { r->render(this) ;}

    return postRender();
}

GLuint Canvas::postRender()
{
    if(!cursor) { cursor = RenderablePoint::create(vec3(0,0,0)) ;}

    {
        const dmat4 m = translate(dmat4(1.0), cursorPosWC);
        
        const dmat4 MV  = view * m;
        const dmat4 MVP = projection * MV;

        cursor->render(MVP, MV);
    }

    if(useFrameBuffer) { return frameBuffer->getTextureID() ;}

    return 0;
}


GLuint Canvas::getTextureID() const
{
    if(!frameBuffer)
    {
        dmess("Error no frameBuffer!");

        return 0;
    }

    return frameBuffer->getTextureID();
}

bool Canvas::setWantMouseCapture(const bool wantMouseCapture)
{
    return this->wantMouseCapture = wantMouseCapture;
}

void Canvas::onMouseButton(GLFWwindow * window, const int button, const int action, const int mods)
{
    if(!enabled) { return ;}
}

void Canvas::onMousePosition(GLFWwindow * window, const vec2 & mousePos)
{
    if(!enabled) { return ;}

    const vec2 pos = mousePos - vec2(upperLeft);

    trackBallInteractor->setClickPoint(pos.x, pos.y);
    trackBallInteractor->update();

    // From: http://antongerdelan.net/opengl/raycasting.html
    const vec4 rayClip = vec4(  (2.0f * pos.x) / size.x - 1.0f,
                                1.0f - (2.0f * pos.y) / size.y, -1.0, 1.0);

    dvec4 rayEye = inverse(projection) * rayClip;
    
    rayEye = dvec4(rayEye.x, rayEye.y, -1.0, 0.0);
    
    const dvec3 rayWor = normalize(inverse(getCamera()->getMatrixConstRef()) * rayEye);
    
    const dvec3 eye = getCamera()->getEye();

    const dvec3 n(0,0,1);

    const double t = -dot(eye, n) / dot(rayWor, n);

    cursorPosWC = eye + rayWor * t;
}

void Canvas::onMouseScroll(GLFWwindow * window, const vec2 & mouseScroll)
{
    if(!enabled || !wantMouseCapture) { return ;}

    int state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

    if (state == GLFW_PRESS)
    {
        // TODO This is not working correctly
        lastShiftKeyDownMousePos += ivec2(mouseScroll.x, 0);

        trackBallInteractor->setClickPoint(lastShiftKeyDownMousePos.x % size.x, lastShiftKeyDownMousePos.y % size.y);
        trackBallInteractor->update();

        return;
    }

    trackBallInteractor->setScrollDirection(mouseScroll.y > 0);
    trackBallInteractor->update();
}

void Canvas::onKey(GLFWwindow * window, const int key, const int scancode, const int action, const int mods)
{
    if(!enabled || !wantMouseCapture) { return ;}

    switch(key)
    {
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_LEFT_ALT:

            trackBallInteractor->setLeftClicked(action);

            if(action)
            {
                double xPos;
                double yPos;
                glfwGetCursorPos(window, &xPos, &yPos);
                lastShiftKeyDownMousePos = ivec2(xPos, yPos);

                //dmess("lastShiftKeyDownMousePos " << lastShiftKeyDownMousePos);
            }

            break;
    }

    switch(key)
    {
        case GLFW_KEY_LEFT_SHIFT: trackBallInteractor->setMotionLeftClick(ARC); break;
        case GLFW_KEY_LEFT_ALT:   trackBallInteractor->setMotionLeftClick(PAN); break;
    }
}

void Canvas::onChar(GLFWwindow * window, const size_t c)
{
    if(!enabled) { return ;}
}

Renderable * Canvas::addRenderiable(Renderable * renderiable)
{
    lock_guard<mutex> _(renderiablesMutex);

    renderiables.push_back(renderiable);

    renderiable->addOnDeleteCallback([this](Renderable * r)
    {
        lock_guard<mutex> _(renderiablesMutex);

        renderiables.remove(r);
    });
    
    return renderiable;
}

vec4 Canvas::setClearColor(const vec4 & clearColor) { return this->clearColor = clearColor ;}

Camera * Canvas::getCamera() const { return trackBallInteractor->getCamera() ;}

TrackBallInteractor * Canvas::getTrackBallInteractor() const { return trackBallInteractor ;}

dmat4 Canvas::getView()       const { return view       ;}
dmat4 Canvas::getModel()      const { return model      ;}
dmat4 Canvas::getProjection() const { return projection ;}
dmat4 Canvas::getMVP()        const { return MVP        ;}
dmat4 Canvas::getMV()         const { return MV         ;}

const dmat4 & Canvas::getViewRef()       const { return view       ;}
const dmat4 & Canvas::getModelRef()      const { return model      ;}
const dmat4 & Canvas::getProjectionRef() const { return projection ;}
const dmat4 & Canvas::getMVP_Ref()       const { return MVP        ;}
const dmat4 & Canvas::getMV_Ref()        const { return MV         ;}

SkyBox * Canvas::setSkyBox(SkyBox * skyBox) { return this->skyBox = skyBox ;}
SkyBox * Canvas::getSkyBox() const          { return skyBox ;}

const list<Renderable *> & Canvas::getRenderiablesRef() const { return renderiables ;}

list<Renderable *> Canvas::getRenderiables() const { return renderiables ;}

vector<Canvas *> Canvas::getInstances() { return instances ;}

bool Canvas::setEnabled(const bool enabled) { return this->enabled = enabled ;}
bool Canvas::getEnabled() const             { return enabled ;}

dvec3 Canvas::getCursorPosWC() const { return cursorPosWC ;}

#ifdef __EMSCRIPTEN__

EMSCRIPTEN_BINDINGS(CanvasBindings)
{
    register_vector<Canvas *>("VectorCanvasPtr");
    class_<Canvas>("Canvas")
    .function("getTextureID", &Canvas::getTextureID)
    .class_function("getInstances", &Canvas::getInstances)
    ;
}

#endif