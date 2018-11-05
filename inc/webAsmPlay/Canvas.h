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
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
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

    virtual void setArea(const glm::ivec2 & upperLeft, const glm::ivec2 & size);

    virtual GLuint render();

    GLuint getTextureID() const;

    bool setWantMouseCapture(const bool wantMouseCapture);

    virtual void onMouseButton(GLFWwindow * window, const int button, const int action, const int mods);

    virtual void onMousePosition(GLFWwindow * window, const glm::vec2 & mousePos);

    virtual void onMouseScroll(GLFWwindow * window, const glm::vec2 & mouseScroll);

    virtual void onKey(GLFWwindow * window, const int key, const int scancode, const int action, const int mods);

    virtual void onChar(GLFWwindow * window, const size_t c);

    rsmz::Camera * getCamera() const;

    rsmz::TrackBallInteractor * getTrackBallInteractor() const;

    Renderable * addRenderiable(Renderable * renderiable);

    glm::vec4 setClearColor(const glm::vec4 & clearColor);

    glm::dmat4 getView() const;
    glm::dmat4 getModel() const;
    glm::dmat4 getProjection() const;
    glm::dmat4 getMVP() const;
    glm::dmat4 getMV() const;

    const glm::dmat4 & getViewRef() const;
    const glm::dmat4 & getModelRef() const;
    const glm::dmat4 & getProjectionRef() const;
    const glm::dmat4 & getMVP_Ref() const;
    const glm::dmat4 & getMV_Ref() const;

    SkyBox * setSkyBox(SkyBox * skyBox);
    SkyBox * getSkyBox() const;

    const std::list<Renderable *> & getRenderiablesRef() const;

    std::list<Renderable *> getRenderiables() const;

    static std::vector<Canvas *> getInstances();

    bool setEnabled(const bool enabled);
    bool getEnabled() const;
    
    glm::dvec3 getCursorPosWC() const;

protected:

    bool preRender();

    GLuint postRender();

    glm::ivec2 upperLeft;
    glm::ivec2 size;

private:

    rsmz::TrackBallInteractor * trackBallInteractor;

    FrameBuffer * frameBuffer;

    bool wantMouseCapture;

    glm::ivec2 lastShiftKeyDownMousePos;

    std::list<Renderable *> renderiables;

    const bool useFrameBuffer;

    glm::vec4 clearColor;

    glm::dmat4 view;
    glm::dmat4 model;
    glm::dmat4 projection;
    glm::dmat4 MV;
    glm::dmat4 MVP;
    
    glm::dvec3 cursorPosWC;

    SkyBox * skyBox;

    bool enabled;

    std::mutex renderiablesMutex;

    Renderable * cursor;
};

#endif // __WEB_ASM_PLAY_CANVAS_H__