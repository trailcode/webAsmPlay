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
#include <glm/mat4x4.hpp>
#include <tceGeom/vec2.h>
#include "Types.h"

namespace rsmz
{
    class Camera;
    class TrackBallInteractor;
}

class FrameBuffer;
class GLFWwindow;
class Renderable;
class SkyBox;

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

    rsmz::TrackBallInteractor * getTrackBallInteractor() const;

    Renderable * addRenderiable(Renderable * renderiable);

    glm::vec4 setClearColor(const glm::vec4 & clearColor);

    glm::mat4 getView() const;
    glm::mat4 getModel() const;
    glm::mat4 getProjection() const;
    glm::mat4 getMVP() const;

    const glm::mat4 & getViewRef() const;
    const glm::mat4 & getModelRef() const;
    const glm::mat4 & getProjectionRef() const;
    const glm::mat4 & getMVP_Ref() const;

    SkyBox * setSkyBox(SkyBox * skyBox);
    SkyBox * getSkyBox() const;

    const std::list<Renderable *> & getRenderiablesRef() const;

    std::list<Renderable *> getRenderiables() const;

    static std::vector<Canvas *> getInstances();

    bool setEnabled(const bool enabled);
    bool getEnabled() const;
    
    glm::vec3 getCursorPosWC() const;

private:

    rsmz::TrackBallInteractor * trackBallInteractor;

    FrameBuffer * frameBuffer;

    tce::geom::Vec2i upperLeft;
    tce::geom::Vec2i size;

    bool wantMouseCapture;

    tce::geom::Vec2i lastShiftKeyDownMousePos;

    std::list<Renderable *> renderiables;

    const bool useFrameBuffer;

    glm::vec4 clearColor;

    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 projection;
    glm::mat4 MVP;
    
    glm::vec3 cursorPosWC;

    SkyBox * skyBox;

    bool enabled;

    std::mutex renderiablesMutex;

    Renderable * cursor;
};

#endif // __WEB_ASM_PLAY_CANVAS_H__