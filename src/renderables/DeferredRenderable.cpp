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

#include <glm/gtx/transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>

using namespace std;
using namespace glm;

namespace
{
    vector<GLfloat> g_vertsAndColors;
    vector<GLuint>  g_triangleIndices;
    vector<GLuint>  g_lineIndices;

    inline void addTriangle(const vec3  & A,
                            const vec3  & B,
                            const vec3  & C,
                            const vec4  & color,
                            size_t      & startIndex,
                            GLfloat    *& vertsAndColorsPtr,
                            GLuint     *& triangleIndicesPtr)
    {
        memcpy(vertsAndColorsPtr, value_ptr(A), sizeof(GLfloat) * 3); vertsAndColorsPtr += 3;
        memcpy(vertsAndColorsPtr, value_ptr(B), sizeof(GLfloat) * 3); vertsAndColorsPtr += 3;
        memcpy(vertsAndColorsPtr, value_ptr(C), sizeof(GLfloat) * 3); vertsAndColorsPtr += 3;

        memcpy(vertsAndColorsPtr, value_ptr(color), sizeof(GLfloat) * 4); vertsAndColorsPtr += 4;

        *triangleIndicesPtr = (GLuint)startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = (GLuint)startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = (GLuint)startIndex++; ++triangleIndicesPtr;
    }
}

DeferredRenderable * DeferredRenderable::createFromQueued(const dmat4 & trans)
{
	if (!g_vertsAndColors.size()) { return NULL; }

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
    for(size_t i = 0; i < g_vertsAndColors.size(); i += 7)
    {
        const vec3 pos(trans * dvec4(*(vec3 *)&g_vertsAndColors[i], 1.0));

        memcpy(&g_vertsAndColors[i], &pos, sizeof(vec3));
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &ebo2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, g_vertsAndColors.size() * sizeof(GLfloat), &g_vertsAndColors[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * g_triangleIndices.size(), &g_triangleIndices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * g_lineIndices.size(), &g_lineIndices[0], GL_STATIC_DRAW);

    // TODO use VertexArrayObject

    const size_t sizeVertex = 3;
    const size_t sizeColor = 4;

    const size_t totalSize = (sizeVertex + sizeColor) * sizeof(GLfloat);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, sizeVertex, GL_FLOAT, GL_FALSE, totalSize, 0);

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, sizeColor, GL_FLOAT, GL_FALSE, totalSize, (void *)((sizeVertex) * sizeof(GLfloat)));

    DeferredRenderable * ret = new DeferredRenderable(vao, ebo, ebo2, vbo, (GLuint)g_triangleIndices.size(), (GLuint)g_lineIndices.size());

    g_vertsAndColors .clear();
    g_triangleIndices.clear();
    g_lineIndices    .clear();

    return ret;
}

void DeferredRenderable::setFromQueued(const glm::dmat4& trans)
{
	for (size_t i = 0; i < g_vertsAndColors.size(); i += 7)
	{
		const vec3 pos(trans * dvec4(*(vec3*)& g_vertsAndColors[i], 1.0));

		memcpy(&g_vertsAndColors[i], &pos, sizeof(vec3));
	}

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, g_vertsAndColors.size() * sizeof(GLfloat), &g_vertsAndColors[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * g_triangleIndices.size(), &g_triangleIndices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * g_lineIndices.size(), &g_lineIndices[0], GL_DYNAMIC_DRAW);

	const size_t sizeVertex = 3;
	const size_t sizeColor = 4;

	const size_t totalSize = (sizeVertex + sizeColor) * sizeof(GLfloat);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, sizeVertex, GL_FLOAT, GL_FALSE, totalSize, 0);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, sizeColor, GL_FLOAT, GL_FALSE, totalSize, (void*)((sizeVertex) * sizeof(GLfloat)));

	m_numTriIndices		= (GLuint)g_triangleIndices.size();
	m_numLineIndices	= (GLuint)g_lineIndices.size();

	g_vertsAndColors .clear();
	g_triangleIndices.clear();
	g_lineIndices    .clear();
}

DeferredRenderable::DeferredRenderable( const GLuint & vao,
                                        const GLuint & ebo,
                                        const GLuint & ebo2,
                                        const GLuint & vbo,
                                        const GLuint & numTriIndices,
                                        const GLuint & numLineIndices) : Renderable      (false,
                                                                                          GUI::s_renderSettingsFillPolygons,
																						  GUI::s_renderSettingsRenderPolygonOutlines),
                                                                         m_vao           (vao),
                                                                         m_ebo           (ebo),
                                                                         m_ebo2          (ebo2),
                                                                         m_vbo           (vbo),
                                                                         m_numTriIndices (numTriIndices),
                                                                         m_numLineIndices(numLineIndices)
{
    setShader(ColorVertexShader::getDefaultInstance());
}

DeferredRenderable::~DeferredRenderable()
{
	glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers     (1, &m_vbo);
    glDeleteBuffers     (1, &m_ebo);
    glDeleteBuffers     (1, &m_ebo2);
}

namespace
{
    inline void addVert(const vec3 & v, const vec4 & color)
    {
        g_vertsAndColors.push_back(float(v.x * 0.1));
        g_vertsAndColors.push_back(float(v.y * 0.1));
        g_vertsAndColors.push_back(float(v.z * 0.1));
        g_vertsAndColors.push_back(float(color.x));
        g_vertsAndColors.push_back(float(color.y));
        g_vertsAndColors.push_back(float(color.z));
        g_vertsAndColors.push_back(float(color.w));
    }
}

void DeferredRenderable::addLine(const vec3 & A,
                                 const vec3 & B,
                                 const vec4 & color)
{
    const size_t index = g_vertsAndColors.size() / 7;

    addVert(A, color);
    addVert(B, color);
    
    g_lineIndices.push_back((uint32_t)index + 0);
    g_lineIndices.push_back((uint32_t)index + 1);
}

void DeferredRenderable::addTriangle(const vec3 & A,
                                     const vec3 & B,
                                     const vec3 & C,
                                     const vec4 & color)
{
    const size_t index = g_vertsAndColors.size() / 7;

    addVert(A, color);
    addVert(B, color);
    addVert(C, color);

    g_triangleIndices.push_back((uint32_t)index + 0);
    g_triangleIndices.push_back((uint32_t)index + 1);
    g_triangleIndices.push_back((uint32_t)index + 2);
}

void DeferredRenderable::addQuadrangle( const vec3 & A,
                                        const vec3 & B,
                                        const vec3 & C,
                                        const vec3 & D,
                                        const vec4 & color)
{
    addVert(A, color);
    addVert(B, color);
    addVert(C, color);
    addVert(A, color);
    addVert(C, color);
    addVert(D, color);

	const size_t index = g_vertsAndColors.size() / 7;

    g_triangleIndices.push_back((uint32_t)index + 0);
    g_triangleIndices.push_back((uint32_t)index + 1);
    g_triangleIndices.push_back((uint32_t)index + 2);
    g_triangleIndices.push_back((uint32_t)index + 3);
    g_triangleIndices.push_back((uint32_t)index + 4);
    g_triangleIndices.push_back((uint32_t)index + 5);
}

void DeferredRenderable::render(Canvas * canvas, const size_t renderStage)
{
    if(!m_shader->shouldRender(false, renderStage)) { return ;}

    glBindVertexArray(					  m_vao);
    glBindBuffer(GL_ARRAY_BUFFER,         m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glDisable(GL_DEPTH_TEST);

    canvas->pushModel(rotate(canvas->getModelRef(), radians(-90.0), dvec3(1, 0, 0)));

    m_shader->bind(canvas, false, renderStage);
    
    glDrawElements(GL_TRIANGLES, m_numTriIndices, GL_UNSIGNED_INT, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);

    glDrawElements(GL_LINES, m_numLineIndices, GL_UNSIGNED_INT, NULL);

    canvas->popMVP();
}