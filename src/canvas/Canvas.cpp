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
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/TrackBallInteractor.h>
#include <webAsmPlay/ColorSymbology.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Frustum.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/renderables/RenderableMesh.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/renderables/SkyBox.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/shaders/SsaoShader.h>
#include <webAsmPlay/canvas/Canvas.h>

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

	struct uniforms_block
	{
		mat4 model;
		mat4 view;
		mat4 proj;
		mat4 modelView;
		mat4 modelViewProj;
	};
}

GLuint          uniforms_buffer = 0;

Canvas::Canvas( const bool   useFrameBuffer,
                const vec4 & clearColor) :  m_useFrameBuffer		(useFrameBuffer),
                                            m_clearColor			(clearColor),
                                            m_trackBallInteractor	(new TrackBallInteractor()),
											m_frustum				(new Frustum())
{
    instances.push_back(this);

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER,  uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);
}

Canvas::~Canvas()
{
    delete m_trackBallInteractor;
    delete m_frameBuffer;
	delete m_frustum;
    // TODO remove from instances!
}

ivec2 Canvas::setFrameBufferSize(const ivec2 & fbSize, const ivec2 & upperLeft)
{
	m_upperLeft = upperLeft;
	m_size      = fbSize;

	m_trackBallInteractor->setScreenSize((float)fbSize.x, (float)fbSize.y);

	if (!m_auxFrameBuffer)
	{
		m_auxFrameBuffer = new FrameBuffer(	fbSize,
											{FB_Component(GL_COLOR_ATTACHMENT0, GL_RGBA32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)}),
											FB_Component(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
													TexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)})});

		m_gBuffer		 = new FrameBuffer(	fbSize,
											{FB_Component(GL_COLOR_ATTACHMENT0, GL_RGBA32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)}),
											 FB_Component(GL_COLOR_ATTACHMENT1, GL_RGBA32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)}),
											 FB_Component(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
													TexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)})});
	}
	
	m_auxFrameBuffer->setBufferSize(fbSize);
	m_gBuffer		->setBufferSize(fbSize);

    return m_frameBufferSize = fbSize;
}

ivec2 Canvas::getFrameBufferSize() const { return m_frameBufferSize ;}

void Canvas::pushModel(const dmat4 & model)
{
    m_stackMVP.push(m_currMVP);

    m_currMVP.m_model = model;

    updateMVP();
}

void Canvas::popMVP()
{
    m_currMVP = m_stackMVP.top();

    m_stackMVP.pop();

	m_frustum->set(m_currMVP.m_MVP);
}

void Canvas::updateMVP()
{
    m_currMVP.m_view        = m_trackBallInteractor->getCamera()->getMatrix();
    m_currMVP.m_projection  = perspective(m_perspectiveFOV, double(m_size.x) / double(m_size.y), 0.0001, 30.0);
	m_currMVP.m_MV			= m_currMVP.m_view			* m_currMVP.m_model;
    m_currMVP.m_MVP			= m_currMVP.m_projection	* m_currMVP.m_MV;

	m_frustum->set(m_currMVP.m_MVP);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);

	uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
																0,
																sizeof(uniforms_block),
																GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	block->model			= m_currMVP.m_model;
	block->view				= m_currMVP.m_view;
	block->proj				= m_currMVP.m_projection;
	block->modelView		= m_currMVP.m_MV;
	block->modelViewProj	= m_currMVP.m_MVP;

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

bool Canvas::preRender()
{
    if(!m_enabled) { return false ;}

    if(!m_size.x || !m_size.y)
    {
        dmess("Error canvas size is not valid!");

        return false;
    }

    Camera * camera = m_trackBallInteractor->getCamera();

    updateMVP();

    ColorDistanceDepthShader3D::getDefaultInstance()->setLightPos(camera->getEyeConstRef());

    if(m_useFrameBuffer)
    {
		if (!m_frameBuffer)
		{
			m_frameBuffer = new FrameBuffer(	m_frameBufferSize,
												{FB_Component(GL_COLOR_ATTACHMENT0, GL_RGBA32F,				
													{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
														TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)}),
												FB_Component(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F,
													{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
														TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST),
														TexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
														TexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)})});
		}

		m_frameBuffer->setBufferSize(m_size);

		m_frameBuffer->bind();

        glViewport(0, 0, m_frameBufferSize.x, m_frameBufferSize.y);
    }

	glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	++m_frameNumber;

    return true;
}

extern GLuint quad_vao;

GLuint Canvas::render()
{
    if(!preRender()) { return 0 ;}

    lock_guard<mutex> _(m_renderiablesMutex);

	m_gBuffer->bind();

	vector<GLenum> m_drawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});

	glDrawBuffers(m_drawBuffers.size(), &m_drawBuffers[0]);

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat one = 1.0f;

	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_COLOR, 1, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	ColorDistanceDepthShader3D	::getDefaultInstance()->setColorSymbology(ColorSymbology::getInstance("defaultMesh"));
	ColorDistanceShader			::getDefaultInstance()->setColorSymbology(ColorSymbology::getInstance("defaultPolygon"));

	for(const auto r : m_rasters)	{ r->render(this, 0) ;}
	for(const auto r : m_polygons)	{ r->render(this, 1) ;}
    for(const auto r : m_meshes)	{ r->render(this, 1) ;}

	{
		vector<GLenum> m_drawBuffers({ GL_COLOR_ATTACHMENT0 });

		glDrawBuffers(m_drawBuffers.size(), &m_drawBuffers[0]);
	}

    for(const auto r : m_lineStrings)         { r->render(this, 0) ;}
    for(const auto r : m_points)              { r->render(this, 0) ;}
    for(const auto r : m_deferredRenderables) { r->render(this, 0) ;} 
    for(const auto r : m_meshes)              { r->render(this, 0) ;}
	
	m_gBuffer->unbind();
	
	if(m_skyBox) { m_skyBox->render(this) ;}

	SsaoShader::getDefaultInstance()->setColorTextureID			(m_gBuffer->getTextureID(0));
	SsaoShader::getDefaultInstance()->setNormalDepthTextureID	(m_gBuffer->getTextureID(1));

	SsaoShader::getDefaultInstance()->bind(this, false, 0);

	glDisable(GL_DEPTH_TEST);
	
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    return postRender();
}

GLuint Canvas::postRender()
{
	renderCursor(m_cursorPosWC);

	if(m_useFrameBuffer) { return m_frameBuffer->getTextureID() ;}

	return 0;
}

dvec2 Canvas::renderCursor(const dvec2 & pos)
{
    if(!m_cursor)
	{
		m_cursor = RenderablePoint::create(vec3(0,0,0));

		m_cursor->ensureVAO();
	}

    pushModel(translate(dmat4(1.0), dvec3(pos, 0.0)));

    m_cursor->render(this, 0);

    popMVP();
    
    return pos;
}

GLuint Canvas::getTextureID() const
{
    if(!m_frameBuffer)
    {
        dmess("Error no frameBuffer!");

        return 0;
    }

    return m_frameBuffer->getTextureID();
}

bool Canvas::setWantMouseCapture(const bool wantMouseCapture) { return m_wantMouseCapture = wantMouseCapture ;}

void Canvas::onMouseButton(GLFWwindow * window, const int button, const int action, const int mods)
{
    if(!m_enabled) { return ;}
}

void Canvas::onMousePosition(GLFWwindow * window, const vec2 & mousePos)
{
    if(!m_enabled) { return ;}

    m_trackBallInteractor->setClickPoint(mousePos.x, mousePos.y);
    m_trackBallInteractor->update();

    // From: http://antongerdelan.net/opengl/raycasting.html
    const vec4 rayClip = vec4(		   (2.0f * mousePos.x) / m_size.x - 1.0f,
                                1.0f - (2.0f * mousePos.y) / m_size.y, -1.0, 1.0);

    dvec4 rayEye = inverse(m_currMVP.m_projection) * rayClip;
    
    rayEye = dvec4(rayEye.x, rayEye.y, -1.0, 0.0);
    
    const dvec3 rayWor = normalize(inverse(getCamera()->getMatrixConstRef()) * rayEye);
    
    const dvec3 eye = getCamera()->getEye();

    const dvec3 n(0,0,1);

    const double t = -dot(eye, n) / dot(rayWor, n);

    m_cursorPosWC = eye + rayWor * t;
}

void Canvas::onMouseScroll(GLFWwindow * window, const vec2 & mouseScroll)
{
    if(!m_enabled || !m_wantMouseCapture) { return ;}

    int state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

    if (state == GLFW_PRESS)
    {
        // TODO This is not working correctly
        m_lastShiftKeyDownMousePos += ivec2(mouseScroll.x, 0);

        m_trackBallInteractor->setClickPoint(m_lastShiftKeyDownMousePos.x % m_size.x, m_lastShiftKeyDownMousePos.y % m_size.y);
        m_trackBallInteractor->update();

        return;
    }

    //trackBallInteractor->setSpeed(fabs(mouseScroll.y) * 0.1);

    m_trackBallInteractor->setScrollDirection(mouseScroll.y > 0);
    m_trackBallInteractor->update();
}

void Canvas::onKey(GLFWwindow * window, const int key, const int scancode, const int action, const int mods)
{
    if(!m_enabled || !m_wantMouseCapture) { return ;}

    switch(key)
    {
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_LEFT_ALT:
		case GLFW_KEY_LEFT_CONTROL:

            m_trackBallInteractor->setLeftClicked(action);

            if(action)
            {
                double xPos;
                double yPos;
                glfwGetCursorPos(window, &xPos, &yPos);
                m_lastShiftKeyDownMousePos = ivec2(xPos, yPos);

                //dmess("lastShiftKeyDownMousePos " << lastShiftKeyDownMousePos);
            }

            break;
    }

    switch(key)
    {
        case GLFW_KEY_LEFT_SHIFT:	m_trackBallInteractor->setMotionLeftClick(ARC);  break;
        case GLFW_KEY_LEFT_ALT:		m_trackBallInteractor->setMotionLeftClick(PAN);  break;
		case GLFW_KEY_LEFT_CONTROL: m_trackBallInteractor->setMotionLeftClick(ZOOM); break;
    }
}

void Canvas::onChar(GLFWwindow * window, const size_t c)
{
    if(!m_enabled) { return ;}
}

Renderable * Canvas::addRenderable(Renderable * renderiable, const bool ensureVAO)
{
	if (ensureVAO)
	{
		GUI::guiSync([renderiable]() { renderiable->ensureVAO(); }, true);
	}

    if(dynamic_cast<DeferredRenderable   *>(renderiable)) { return addRenderable(m_deferredRenderables, renderiable) ;}
    if(dynamic_cast<RenderableLineString *>(renderiable)) { return addRenderable(m_lineStrings,         renderiable) ;}
    if(dynamic_cast<RenderablePolygon    *>(renderiable)) { return addRenderable(m_polygons,            renderiable) ;}
    if(dynamic_cast<RenderablePoint      *>(renderiable)) { return addRenderable(m_points,              renderiable) ;}
    if(dynamic_cast<RenderableMesh       *>(renderiable)) { return addRenderable(m_meshes,              renderiable) ;}
    if(dynamic_cast<RenderableBingMap    *>(renderiable)) { return addRenderable(m_rasters,             renderiable) ;}

    dmessError("Error! Implement!");
    
    return renderiable;
}

Renderable * Canvas::addRenderable(list<Renderable *> & container, Renderable * renderiable)
{   
    lock_guard<mutex> _(m_renderiablesMutex);

    container.push_back(renderiable);

    renderiable->addOnDeleteCallback([this, &container](Renderable * r)
    {
        lock_guard<mutex> _(m_renderiablesMutex);

        container.remove(r);
    });

    return renderiable;
}

vector<Renderable *> Canvas::getRenderiables() const
{
    vector<Renderable *> ret;

    ret.insert(ret.end(), m_points.begin(),       m_points.end());
    ret.insert(ret.end(), m_lineStrings.begin(),  m_lineStrings.end());
    ret.insert(ret.end(), m_polygons.begin(),     m_polygons.end());
    ret.insert(ret.end(), m_meshes.begin(),       m_meshes.end());
    ret.insert(ret.end(), m_rasters.begin(),      m_rasters.end());

    return ret;
}

const list<Renderable *> & Canvas::getPointsRef()               const { return m_points               ;}
const list<Renderable *> & Canvas::getLineStringsRef()          const { return m_lineStrings          ;}
const list<Renderable *> & Canvas::getPolygonsRef()             const { return m_polygons             ;}
const list<Renderable *> & Canvas::getMeshesRef()               const { return m_meshes               ;}
const list<Renderable *> & Canvas::getDeferredRenderablesRef()  const { return m_deferredRenderables  ;}
const list<Renderable *> & Canvas::getRastersRef()              const { return m_rasters              ;}

vec4 Canvas::setClearColor(const vec4 & clearColor) { return this->m_clearColor = clearColor ;}

Camera * Canvas::getCamera() const { return m_trackBallInteractor->getCamera() ;}

const Frustum * Canvas::getCameraFrustum() const { return m_frustum ;}

TrackBallInteractor * Canvas::getTrackBallInteractor() const { return m_trackBallInteractor ;}

dmat4 Canvas::getView()								const	{ return m_currMVP.m_view		;}
dmat4 Canvas::getModel()							const	{ return m_currMVP.m_model		;}
dmat4 Canvas::getProjection()						const	{ return m_currMVP.m_projection	;}
dmat4 Canvas::getMVP()								const	{ return m_currMVP.m_MVP		;}
dmat4 Canvas::getMV()								const	{ return m_currMVP.m_MV			;}

const dmat4 & Canvas::getViewRef()					const	{ return m_currMVP.m_view		;}
const dmat4 & Canvas::getModelRef()					const	{ return m_currMVP.m_model		;}
const dmat4 & Canvas::getProjectionRef()			const	{ return m_currMVP.m_projection	;}
const dmat4 & Canvas::getMVP_Ref()					const	{ return m_currMVP.m_MVP		;}
const dmat4 & Canvas::getMV_Ref()					const	{ return m_currMVP.m_MV			;}

SkyBox * Canvas::setSkyBox(SkyBox * skyBox)					{ return m_skyBox = skyBox		;}
SkyBox * Canvas::getSkyBox()						const	{ return m_skyBox				;}

bool Canvas::setEnabled(const bool enabled)					{ return m_enabled = enabled	;}
bool Canvas::getEnabled()							const	{ return m_enabled				;}

double Canvas::getPerspectiveFOV()					const	{ return m_perspectiveFOV		;}
double Canvas::setPerspectiveFOV(const double FOV)			{ return m_perspectiveFOV = FOV ;}

vector<Canvas *> Canvas::getInstances()						{ return instances				;}

dvec3 Canvas::getCursorPosWC()						const	{ return m_cursorPosWC			;}

Renderable * Canvas::getCursor()					const	{ return m_cursor				;}

ivec2 Canvas::getUpperLeft()						const	{ return m_upperLeft			;}
ivec2 Canvas::getSize()								const	{ return m_size					;}

FrameBuffer * Canvas::getAuxFrameBuffer()			const	{ return m_auxFrameBuffer		;}
FrameBuffer * Canvas::getG_FrameBuffer()			const	{ return m_gBuffer				;}

size_t Canvas::getFrameNumber()						const	{ return m_frameNumber			;}

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