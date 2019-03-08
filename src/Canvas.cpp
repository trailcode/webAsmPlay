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
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/renderables/RenderableMesh.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
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
                const vec4 & clearColor) :  useFrameBuffer     (useFrameBuffer),
                                            clearColor         (clearColor),
                                            trackBallInteractor(new TrackBallInteractor())
{
    instances.push_back(this);
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

    trackBallInteractor->setScreenSize(size.x, size.y);
}

ivec2 Canvas::setFrameBufferSize(const ivec2 & fbSize)
{
    //if(useFrameBuffer) { frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, fbSize) ;}

    auxFrameBuffer = FrameBuffer::ensureFrameBuffer(auxFrameBuffer, fbSize);

    return frameBufferSize = fbSize;
}

ivec2 Canvas::getFrameBufferSize() const { return frameBufferSize ;}

void Canvas::pushModel(const dmat4 & model)
{
    stackMVP.push(currMVP);

    currMVP.model = model;

    updateMVP();
}

void Canvas::popMVP()
{
    currMVP = stackMVP.top();

    stackMVP.pop();
}

extern vec4 lookat;
extern vec4 pos;   
extern vec4 up;

void Canvas::updateMVP()
{
    currMVP.view        = trackBallInteractor->getCamera()->getMatrix();
    currMVP.projection  = perspective(45.0, double(size.x) / double(size.y), 0.0001, 30.0);
    currMVP.MV          = currMVP.view * currMVP.model;
    currMVP.MVP         = currMVP.projection * currMVP.MV;
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

    updateMVP();

    ColorDistanceShader3D     ::getDefaultInstance()->setLightPos(camera->getEyeConstRef());
    ColorDistanceDepthShader3D::getDefaultInstance()->setLightPos(camera->getEyeConstRef());

    if(useFrameBuffer)
    {
        //frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, size);
        frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, frameBufferSize);

        frameBuffer->bind();
    
        //GL_CHECK(glViewport(0, 0, size.x, size.y));
        GL_CHECK(glViewport(0, 0, frameBufferSize.x, frameBufferSize.y));
    }

    if(skyBox) { skyBox->render(this) ;}

    else
    {
        GL_CHECK(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
    }

    GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

    return true;
}

#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorSymbology.h>

GLuint Canvas::render()
{
    if(!preRender()) { return 0 ;}

    lock_guard<mutex> _(renderiablesMutex);

    if(auxFrameBuffer)
    {
        auxFrameBuffer->bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(const auto r : meshes) { r->render(this, 1) ;}

        glFlush();

        auxFrameBuffer->unbind();
    }

    for(const auto r : rasters) { r->render(this, 0) ;}

    // TODO try to refactor this. Who owns the symbology?
    ColorDistanceShader::getDefaultInstance()->setColorSymbology(ColorSymbology::getInstance("defaultPolygon"));

    for(const auto r : polygons)            { r->render(this, 0) ;}

    ColorDistanceShader::getDefaultInstance()->setColorSymbology(ColorSymbology::getInstance("defaultLinear"));

    for(const auto r : lineStrings)         { r->render(this, 0) ;}
    for(const auto r : points)              { r->render(this, 0) ;}
    for(const auto r : deferredRenderables) { r->render(this, 0) ;} 
    //for(const auto r : meshes)              { r->render(this) ;}

    ColorDistanceShader3D     ::getDefaultInstance()->setColorSymbology(ColorSymbology::getInstance("defaultMesh"));
    ColorDistanceDepthShader3D::getDefaultInstance()->setColorSymbology(ColorSymbology::getInstance("defaultMesh"));

    for(const auto r : meshes)              { r->render(this, 0) ;}

    return postRender();
}

dvec2 Canvas::renderCursor(const dvec2 & pos)
{
    if(!cursor) { cursor = RenderablePoint::create(vec3(0,0,0)) ;}

    pushModel(translate(dmat4(1.0), dvec3(pos, 0.0)));

    cursor->render(this, 0);

    popMVP();
    
    return pos;
}

GLuint Canvas::postRender()
{
    renderCursor(cursorPosWC);

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

    dvec4 rayEye = inverse(currMVP.projection) * rayClip;
    
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

    //trackBallInteractor->setSpeed(fabs(mouseScroll.y) * 0.1);

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

Renderable * Canvas::addRenderable(Renderable * renderiable)
{
    if(dynamic_cast<DeferredRenderable   *>(renderiable)) { return addRenderable(deferredRenderables, renderiable) ;}
    if(dynamic_cast<RenderableLineString *>(renderiable)) { return addRenderable(lineStrings,         renderiable) ;}
    if(dynamic_cast<RenderablePolygon    *>(renderiable)) { return addRenderable(polygons,            renderiable) ;}
    if(dynamic_cast<RenderablePoint      *>(renderiable)) { return addRenderable(points,              renderiable) ;}
    if(dynamic_cast<RenderableMesh       *>(renderiable)) { return addRenderable(meshes,              renderiable) ;}
    if(dynamic_cast<RenderableBingMap    *>(renderiable)) { return addRenderable(rasters,             renderiable) ;}

    dmess("Error! Implement!");
    
    abort();

    return renderiable;
}

Renderable * Canvas::addRenderable(list<Renderable *> & container, Renderable * renderiable)
{   
    lock_guard<mutex> _(renderiablesMutex);

    container.push_back(renderiable);

    renderiable->addOnDeleteCallback([this, &container](Renderable * r)
    {
        lock_guard<mutex> _(renderiablesMutex);

        container.remove(r);
    });

    return renderiable;
}

vector<Renderable *> Canvas::getRenderiables() const
{
    vector<Renderable *> ret;

    ret.insert(ret.end(), points.begin(),       points.end());
    ret.insert(ret.end(), lineStrings.begin(),  lineStrings.end());
    ret.insert(ret.end(), polygons.begin(),     polygons.end());
    ret.insert(ret.end(), meshes.begin(),       meshes.end());
    ret.insert(ret.end(), rasters.begin(),      rasters.end());

    return ret;
}

const list<Renderable *> & Canvas::getPointsRef()               const { return points               ;}
const list<Renderable *> & Canvas::getLineStringsRef()          const { return lineStrings          ;}
const list<Renderable *> & Canvas::getPolygonsRef()             const { return polygons             ;}
const list<Renderable *> & Canvas::getMeshesRef()               const { return meshes               ;}
const list<Renderable *> & Canvas::getDeferredRenderablesRef()  const { return deferredRenderables  ;}
const list<Renderable *> & Canvas::getRastersRef()              const { return rasters              ;}

vec4 Canvas::setClearColor(const vec4 & clearColor) { return this->clearColor = clearColor ;}

Camera * Canvas::getCamera() const { return trackBallInteractor->getCamera() ;}

TrackBallInteractor * Canvas::getTrackBallInteractor() const { return trackBallInteractor ;}

dmat4 Canvas::getView()                  const { return currMVP.view       ;}
dmat4 Canvas::getModel()                 const { return currMVP.model      ;}
dmat4 Canvas::getProjection()            const { return currMVP.projection ;}
dmat4 Canvas::getMVP()                   const { return currMVP.MVP        ;}
dmat4 Canvas::getMV()                    const { return currMVP.MV         ;}

const dmat4 & Canvas::getViewRef()       const { return currMVP.view       ;}
const dmat4 & Canvas::getModelRef()      const { return currMVP.model      ;}
const dmat4 & Canvas::getProjectionRef() const { return currMVP.projection ;}
const dmat4 & Canvas::getMVP_Ref()       const { return currMVP.MVP        ;}
const dmat4 & Canvas::getMV_Ref()        const { return currMVP.MV         ;}

SkyBox * Canvas::setSkyBox(SkyBox * skyBox) { return this->skyBox = skyBox ;}
SkyBox * Canvas::getSkyBox() const          { return skyBox ;}

vector<Canvas *> Canvas::getInstances() { return instances ;}

bool Canvas::setEnabled(const bool enabled) { return this->enabled = enabled ;}
bool Canvas::getEnabled() const             { return enabled ;}

dvec3 Canvas::getCursorPosWC() const { return cursorPosWC ;}

Renderable * Canvas::getCursor() const { return cursor ;}

FrameBuffer * Canvas::getAuxFrameBuffer() const { return auxFrameBuffer ;}

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