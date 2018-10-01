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

    Canvas();
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

private:

    rsmz::TrackBallInteractor * trackBallInteractor;

    FrameBuffer * frameBuffer;

    tce::geom::Vec2i upperLeft;
    tce::geom::Vec2i size;

    bool wantMouseCapture;

    tce::geom::Vec2i lastShiftKeyDownMousePos;

    std::list<Renderiable *> renderiables;
};

#endif // __WEB_ASM_PLAY_CANVAS_H__