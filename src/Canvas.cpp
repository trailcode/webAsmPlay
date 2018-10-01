#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/Renderiable.h>
#include <webAsmPlay/Canvas.h>

using namespace std;
using namespace rsmz;
using namespace glm;
using namespace geos::geom;
using namespace tce::geom;

Canvas::Canvas() :  trackBallInteractor (NULL),
                    frameBuffer         (NULL),
                    wantMouseCapture    (true)
{
    trackBallInteractor = new TrackBallInteractor();
}

Canvas::~Canvas()
{
    delete trackBallInteractor;
    delete frameBuffer;
}

void Canvas::setArea(const Vec2i & upperLeft, const Vec2i & size)
{
    this->upperLeft = upperLeft;
    this->size      = size;

    frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, size);

    trackBallInteractor->setScreenSize(size.x, size.y);
}

GLuint Canvas::render()
{
    Camera * camera = trackBallInteractor->getCamera();

    const mat4 view         = camera->getMatrix();
    const mat4 model        = mat4(1.0);
    const mat4 projection   = perspective(45.0, double(size.x) / double(size.y), 0.1, 100.0);
    const mat4 MVP          = projection * view * model;

    frameBuffer->bind();
    
    glViewport(0,0,size.x,size.y);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    Polygon * pp = scopedGeosGeometry(GeosUtil::makeBox(-0.1,-0.1,0.1,0.1));

    Polygon * p = scopedGeosGeometry(GeosUtil::makeBox(-0.5,-0.5,0.5,0.5));

    p = scopedGeosGeometry(dynamic_cast<Polygon *>(p->buffer(0.1)));

    p = scopedGeosGeometry(dynamic_cast<Polygon *>(p->difference(pp)));

    unique_ptr<Renderiable> r(Renderiable::create(p));

    if(r)
    {
        r->setFillColor(vec4(0.3,0.3,0,1));
        
        r->setOutlineColor(vec4(1,0,0,1));

        r->render(MVP);
    }

    /*
    unique_ptr<GeosRenderiable> rr(GeosRenderiable::create(p->getExteriorRing()));

    if(r)
    {
        rr->setFillColor(vec4(1,0,1,1));
        
        rr->render(MVP);
    }
    */

    return frameBuffer->getTextureID();
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

}

void Canvas::onMousePosition(GLFWwindow * window, const Vec2d & mousePos)
{
    const Vec2d pos = mousePos - upperLeft;

    //dmess("mousePos " << pos);

    trackBallInteractor->setClickPoint(pos.x, pos.y);
    trackBallInteractor->update();
}

void Canvas::onMouseScroll(GLFWwindow * window, const Vec2d & mouseScroll)
{
    if(!wantMouseCapture) { return ;}

    int state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

    if (state == GLFW_PRESS)
    {
        // TODO This is not working correctly
        lastShiftKeyDownMousePos += Vec2d(mouseScroll.x, 0);
        trackBallInteractor->setClickPoint(lastShiftKeyDownMousePos.x % size.x, lastShiftKeyDownMousePos.y % size.y);
        trackBallInteractor->update();

        return;
    }

    trackBallInteractor->setScrollDirection(mouseScroll.y > 0);
    trackBallInteractor->update();
}

void Canvas::onKey(GLFWwindow * window, const int key, const int scancode, const int action, const int mods)
{
    if(!wantMouseCapture) { return ;}

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
                lastShiftKeyDownMousePos = Vec2i(xPos, yPos);

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

}

Camera * Canvas::getCamera() const { return trackBallInteractor->getCamera() ;}
