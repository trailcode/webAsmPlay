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

	#ifdef USE_GL_ES3
	// OpenGL ES 3
	#include <GLES3/gl3.h>  // Use GL ES 3
	#else
	// Regular OpenGL
	// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
	// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
	// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
	#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	#include <GL/gl3w.h>
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	#include <GL/glew.h>
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	#include <glad/glad.h>
	#else
	#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
	#endif
	#endif

#endif

#include <list>
#include <vector>
#include <stack>
#include <mutex>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/GUI/ImguiInclude.h>

class FrameBuffer;
//class GLFWwindow;
class Renderable;
class SkyBox;

namespace rsmz
{
    class Camera;
    class TrackBallInteractor;
}

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

    Renderable * addRenderable(Renderable * renderiable);

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

    void pushModel(const glm::dmat4 & model);
    void popMVP();

    SkyBox * setSkyBox(SkyBox * skyBox);
    SkyBox * getSkyBox() const;

    std::vector<Renderable *> getRenderiables() const;

    static std::vector<Canvas *> getInstances();

    bool setEnabled(const bool enabled);
    bool getEnabled() const;
    
    glm::dvec3 getCursorPosWC() const;

    Renderable * getCursor() const;

    glm::dvec2 renderCursor(const glm::dvec2 & pos);

protected:

    bool preRender();

    GLuint postRender();

    glm::ivec2 upperLeft;
    glm::ivec2 size;

private:

    Renderable * addRenderable(std::list<Renderable *> & container, Renderable * renderiable);

    void updateMVP();

    rsmz::TrackBallInteractor * trackBallInteractor = NULL;

    FrameBuffer * frameBuffer = NULL;

    bool wantMouseCapture = true;

    glm::ivec2 lastShiftKeyDownMousePos;

    std::list<Renderable *> points;
    std::list<Renderable *> lineStrings;
    std::list<Renderable *> polygons;
    std::list<Renderable *> meshes;
    std::list<Renderable *> deferredRenderables;

    const bool useFrameBuffer;

    glm::vec4 clearColor;

    struct MVP
    {
        glm::dmat4 view;
        glm::dmat4 model         = glm::dmat4(1.0);
        glm::dmat4 projection;
        glm::dmat4 MV;
        glm::dmat4 MVP;
    };

    MVP currMVP;

    std::stack<MVP> stackMVP;
    
    glm::dvec3 cursorPosWC;

    SkyBox * skyBox = NULL;

    bool enabled = true;

    std::mutex renderiablesMutex;

    Renderable * cursor = NULL;
};

#endif // __WEB_ASM_PLAY_CANVAS_H__