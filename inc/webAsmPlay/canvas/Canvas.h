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
#pragma once

#include <list>
#include <vector>
#include <stack>
#include <mutex>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/GUI/ImguiInclude.h>

class FrameBuffer;
class Renderable;
class SkyBox;
class Frustum;

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

    virtual glm::ivec2 setFrameBufferSize(const glm::ivec2 & fbSize, const glm::ivec2 & upperLeft = glm::ivec2(0,0));
    virtual glm::ivec2 getFrameBufferSize() const;

    virtual GLuint render();

    GLuint getTextureID() const;

    bool setWantMouseCapture(const bool wantMouseCapture);

    virtual void onMouseButton(GLFWwindow * window, const int button, const int action, const int mods);

    virtual void onMousePosition(GLFWwindow * window, const glm::vec2 & mousePos);

    virtual void onMouseScroll(GLFWwindow * window, const glm::vec2 & mouseScroll);

    virtual void onKey(GLFWwindow * window, const int key, const int scancode, const int action, const int mods);

    virtual void onChar(GLFWwindow * window, const size_t c);

    rsmz::Camera * getCamera() const;

	const Frustum * getCameraFrustum() const;

    rsmz::TrackBallInteractor * getTrackBallInteractor() const;

    Renderable * addRenderable(Renderable * renderiable, const bool ensureVAO = true);

	Renderable * removeRenderable(Renderable * renderiable);

    glm::vec4 setClearColor(const glm::vec4 & clearColor);

    glm::dmat4 getView()		const;
    glm::dmat4 getModel()		const;
    glm::dmat4 getProjection()	const;
    glm::dmat4 getMVP()			const;
    glm::dmat4 getMV()			const;

    const glm::dmat4 & getViewRef()			const;
    const glm::dmat4 & getModelRef()		const;
    const glm::dmat4 & getProjectionRef()	const;
    const glm::dmat4 & getMVP_Ref()			const;
    const glm::dmat4 & getMV_Ref()			const;

    void pushModel(const glm::dmat4 & model);
    void popMVP();

    SkyBox * setSkyBox(SkyBox * skyBox);
    SkyBox * getSkyBox() const;

    std::vector<Renderable *> getRenderiables() const;

    const std::list<Renderable *> & getPointsRef()				const;
    const std::list<Renderable *> & getLineStringsRef()			const;
    const std::list<Renderable *> & getPolygonsRef()			const;
    const std::list<Renderable *> & getMeshesRef()				const;
    const std::list<Renderable *> & getDeferredRenderablesRef() const;
    const std::list<Renderable *> & getRastersRef()				const;
    const std::list<Renderable *> & getModelsRef()				const;
	const std::list<Renderable *> & getTextLablesRef()			const;

    static std::vector<Canvas *> getInstances();

    bool setEnabled(const bool enabled);
    bool getEnabled() const;
    
    glm::dvec3 getCursorPosWC() const;

    Renderable * getCursor() const;

	glm::ivec2 getUpperLeft() const;
	glm::ivec2 getSize() const;

    glm::dvec2 renderCursor(const glm::dvec2 & pos);

	FrameBuffer * getAuxFrameBuffer() const;
	FrameBuffer * getG_FrameBuffer()  const;

	double getPerspectiveFOV() const;
	double setPerspectiveFOV(const double FOV);

	size_t getFrameNumber() const;

	void addLeftClickListener(const std::function<void(const glm::dvec3 & posWC)> & listener);

	void addMouseMoveListener(const std::function<void(const glm::dvec3 & posWC)> & listener);

	void addPreRenderFunctor(const std::function<void()> & functor);

protected:

    bool preRender();

    GLuint postRender();

    glm::ivec2 m_upperLeft;
    glm::ivec2 m_size = glm::ivec2(1,1);

    glm::ivec2 m_frameBufferSize = glm::ivec2(1,1);

	size_t m_frameNumber = 0;

    Renderable * addRenderable(std::list<Renderable *> & container, Renderable * renderiable);

    void updateMVP();

    rsmz::TrackBallInteractor * m_trackBallInteractor = nullptr;

	FrameBuffer * m_frameBuffer = nullptr;
	FrameBuffer * m_gBuffer		= nullptr;

    bool m_wantMouseCapture = true;

    glm::ivec2 m_lastShiftKeyDownMousePos;

    std::list<Renderable *> m_points;
    std::list<Renderable *> m_lineStrings;
    std::list<Renderable *> m_polygons;
    std::list<Renderable *> m_meshes;
    std::list<Renderable *> m_deferredRenderables;
    std::list<Renderable *> m_rasters;
	std::list<Renderable *> m_models;
	std::list<Renderable *> m_textLabels;

    const bool m_useFrameBuffer;

    glm::vec4 m_clearColor;

    struct MVP
    {
		glm::dmat4 m_model	= glm::dmat4(1.0);
        glm::dmat4 m_view;
        glm::dmat4 m_projection;
        glm::dmat4 m_MV;
        glm::dmat4 m_MVP;
    };

    MVP m_currMVP;

    std::stack<MVP> m_stackMVP;
    
    glm::dvec3 m_cursorPosWC;

    SkyBox * m_skyBox = nullptr;

    bool m_enabled = true;

    std::mutex m_renderiablesMutex;

    Renderable * m_cursor = nullptr;

	FrameBuffer * m_auxFrameBuffer = nullptr;

	Frustum * m_frustum = nullptr;

	double m_perspectiveFOV = 45.0;

	std::vector<std::function<void(const glm::dvec3 & posWC)>> m_leftClickListeners;
	std::vector<std::function<void(const glm::dvec3 & posWC)>> m_mouseMoveListeners;

	std::vector<std::function<void()>> m_preRenderFunctors;
};
