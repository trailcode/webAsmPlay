#ifndef __WEB_ASM_PLAY_CANVAS_H__
#define __WEB_ASM_PLAY_CANVAS_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>    // Initialize with gl3wInit()
#endif

#include <list>
#include <glm/vec4.hpp>
#include <tceGeom/vec2.h>

namespace rsmz
{
    class Camera;
    class TrackBallInteractor;
}

class FrameBuffer;
class GLFWwindow;
class Renderiable;

class Canvas
{
public:

    Canvas( const bool        useFrameBuffer = true,
            const glm::vec4 & clearColor     = glm::vec4(0.5, 0.5, 1, 1));
    virtual ~Canvas();

    void setArea(const tce::geom::Vec2i & upperLeft, const tce::geom::Vec2i & size);

    virtual GLuint render();

    GLuint getTextureID() const;

    bool setWantMouseCapture(const bool wantMouseCapture);

    void onMouseButton(GLFWwindow * window, const int button, const int action, const int mods);

    void onMousePosition(GLFWwindow * window, const tce::geom::Vec2d & mousePos);

    void onMouseScroll(GLFWwindow * window, const tce::geom::Vec2d & mouseScroll);

    void onKey(GLFWwindow * window, const int key, const int scancode, const int action, const int mods);

    void onChar(GLFWwindow * window, const size_t c);

    rsmz::Camera * getCamera() const;

    Renderiable * addRenderiable(Renderiable * renderiable);

    glm::vec4 setClearColor(const glm::vec4 & clearColor);

    glm::mat4 getView() const;
    glm::mat4 getModel() const;
    glm::mat4 getProjection() const;
    glm::mat4 getMVP() const;

    const glm::mat4 & getViewRef() const;
    const glm::mat4 & getModelRef() const;
    const glm::mat4 & getProjectionRef() const;
    const glm::mat4 & getMVP_Ref() const;

private:

    rsmz::TrackBallInteractor * trackBallInteractor;

    FrameBuffer * frameBuffer;

    tce::geom::Vec2i upperLeft;
    tce::geom::Vec2i size;

    bool wantMouseCapture;

    tce::geom::Vec2i lastShiftKeyDownMousePos;

    std::list<Renderiable *> renderiables;

    const bool useFrameBuffer;

    glm::vec4 clearColor;

    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 projection;
    glm::mat4 MVP;
};

#endif // __WEB_ASM_PLAY_CANVAS_H__