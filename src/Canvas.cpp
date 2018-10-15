#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
    #define IMGUI_API
    #include <imgui_impl_glfw_gl3.h>
    #include <emscripten/emscripten.h>
    #include <emscripten/bind.h>
    #include <GL/glu.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
    #define IMGUI_IMPL_API // What about for windows?
    #include <imgui.h>
    #include <imgui_impl_opengl3.h>
    #include <imgui_impl_glfw.h>
#endif // __EMSCRIPTEN__
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/Renderiable.h>
#include <webAsmPlay/SkyBox.h>
#include <webAsmPlay/Canvas.h>

#ifdef __EMSCRIPTEN__

    using namespace emscripten;

#endif

using namespace std;
using namespace rsmz;
using namespace glm;
using namespace geos::geom;
using namespace tce::geom;

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
}

Canvas::~Canvas()
{
    delete trackBallInteractor;
    delete frameBuffer;

    // TODO remove from instances!
}

void Canvas::setArea(const Vec2i & upperLeft, const Vec2i & size)
{
    this->upperLeft = upperLeft;
    this->size      = size;

    if(useFrameBuffer) { frameBuffer = FrameBuffer::ensureFrameBuffer(frameBuffer, size) ;}

    trackBallInteractor->setScreenSize(size.x, size.y);
}

GLuint Canvas::render()
{
    if(!enabled) { return 0 ;}

    if(!size.x || !size.y)
    {
        dmess("Error canvas size is not valid!");

        return 0;
    }

    Camera * camera = trackBallInteractor->getCamera();

    view         = camera->getMatrix();
    model        = mat4(1.0);
    projection   = perspective(45.0, double(size.x) / double(size.y), 0.01, 30.0);
    MVP          = projection * view * model;

    if(useFrameBuffer)
    {
        frameBuffer->bind();
    
        glViewport(0,0,size.x,size.y);
    }

    if(skyBox) { skyBox->render(getCamera()->getMatrix(), getProjectionRef()) ;}

    else
    {
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        glClear(GL_COLOR_BUFFER_BIT);
    }

    lock_guard _(renderiablesMutex);

    for(Renderiable * r : renderiables) { r->render(MVP) ;}

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

void Canvas::onMousePosition(GLFWwindow * window, const Vec2d & mousePos)
{
    if(!enabled) { return ;}

    const Vec2d pos = mousePos - upperLeft;

    //dmess("mousePos " << pos);

    trackBallInteractor->setClickPoint(pos.x, pos.y);
    trackBallInteractor->update();

    /*
    // From: http://antongerdelan.net/opengl/raycasting.html
    float x = (2.0f * mousePos.x) / size.x - 1.0f;
    float y = 1.0f - (2.0f * mousePos.y) / size.y;
    float z = 1.0f;
    vec3 ray_nds = vec3(x, y, z);
    dmess("ray_nds " << ray_nds << " mousePos " << mousePos);
    //vec4 ray_clip = vec4(ray_nds.xy, -1.0, 1.0);
    vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    vec4 ray_eye = inverse(projection) * ray_clip;
    ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
    //vec3 ray_wor = (inverse(getCamera()->getView() ) * ray_eye).xyz;
    //vec3 ray_wor = inverse(view) * ray_eye;
    vec3 ray_wor = inverse(getCamera()->getMatrixConstRef()) * ray_eye;
    ray_wor = normalize(ray_wor); // don't forget to normalise the vector at some point
    //dmess("ray " << ray_wor.x << " " << ray_wor.y << " " << ray_wor.z);
    vec3 eye = getCamera()->getEye();
    //dmess("ray " << ray_wor << " eye " << getCamera()->getEye());
    vec3 n(0,0,1);
    float t = -dot(eye, n) / dot(ray_wor, n);
    //dmess("t " << t);
    vec3 interPos = eye + ray_wor * t;
    //dmess("interPos " << interPos << " " << mousePos << " " << eye);
    */
}

void Canvas::onMouseScroll(GLFWwindow * window, const Vec2d & mouseScroll)
{
    if(!enabled || !wantMouseCapture) { return ;}

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
    if(!enabled) { return ;}
}

Renderiable * Canvas::addRenderiable(Renderiable * renderiable)
{
    lock_guard _(renderiablesMutex);
    
    renderiables.push_back(renderiable);
    
    return renderiable;
}

vec4 Canvas::setClearColor(const vec4 & clearColor) { return this->clearColor = clearColor ;}

Camera * Canvas::getCamera() const { return trackBallInteractor->getCamera() ;}

mat4 Canvas::getView()       const { return view ;}
mat4 Canvas::getModel()      const { return model ;}
mat4 Canvas::getProjection() const { return projection ;}
mat4 Canvas::getMVP()        const { return MVP ;}

const mat4 & Canvas::getViewRef()       const { return view ;}
const mat4 & Canvas::getModelRef()      const { return model ;}
const mat4 & Canvas::getProjectionRef() const { return projection ;}
const mat4 & Canvas::getMVP_Ref()       const { return MVP ;}

SkyBox * Canvas::setSkyBox(SkyBox * skyBox) { return this->skyBox = skyBox ;}
SkyBox * Canvas::getSkyBox() const          { return skyBox ;}

const list<Renderiable *> & Canvas::getRenderiablesRef() const { return renderiables ;}

list<Renderiable *> Canvas::getRenderiables() const { return renderiables ;}

vector<Canvas *> Canvas::getInstances() { return instances ;}

bool Canvas::setEnabled(const bool enabled) { return this->enabled = enabled ;}
bool Canvas::getEnabled() const             { return enabled ;}

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