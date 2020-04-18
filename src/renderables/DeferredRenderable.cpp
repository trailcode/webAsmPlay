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
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>

using namespace std;
using namespace glm;

namespace
{
    vector<GLfloat> a_vertsAndColors	[3];
    vector<GLuint>  a_triangleIndices	[3];
    vector<GLuint>  a_lineIndices		[3];

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

DeferredRenderable * DeferredRenderable::createFromQueued(const size_t slot, const dmat4 & trans)
{
	if (!a_vertsAndColors[slot].size()) { return nullptr; }
	
    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
	/*
    for(size_t i = 0; i < a_vertsAndColors[slot].size(); i += 7)
    {
        const vec3 pos(trans * dvec4(*(vec3 *)&a_vertsAndColors[slot][i], 1.0));

        memcpy(&a_vertsAndColors[slot][i], &pos, sizeof(vec3));
    }
	*/

    glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &ebo2);

	/*
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, a_vertsAndColors[slot].size() * sizeof(GLfloat), &a_vertsAndColors[slot][0], GL_STATIC_DRAW);
    
	if(a_triangleIndices[slot].size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * a_triangleIndices[slot].size(), &a_triangleIndices[slot][0], GL_STATIC_DRAW);
	}
    
	if(a_lineIndices[slot].size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * a_lineIndices[slot].size(), &a_lineIndices[slot][0], GL_STATIC_DRAW);
	}

    // TODO use VertexArrayObject

    const size_t sizeVertex = 3;
    const size_t sizeColor = 4;

    const size_t totalSize = (sizeVertex + sizeColor) * sizeof(GLfloat);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, sizeVertex, GL_FLOAT, GL_FALSE, totalSize, 0);

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, sizeColor, GL_FLOAT, GL_FALSE, totalSize, (void *)((sizeVertex) * sizeof(GLfloat)));
	*/

    auto ret = new DeferredRenderable(vao, ebo, ebo2, vbo, (GLuint)a_triangleIndices[slot].size(), (GLuint)a_lineIndices[slot].size());

	ret->setFromQueued(slot, trans);

	/*
    a_vertsAndColors	[slot].clear();
    a_triangleIndices	[slot].clear();
    a_lineIndices		[slot].clear();
	*/

    return ret;
}

void DeferredRenderable::setFromQueued(const size_t slot, const glm::dmat4& trans)
{
	auto & vertsAndColors	= a_vertsAndColors[slot];
	auto & triangleIndices	= a_triangleIndices[slot];
	auto & lineIndices		= a_lineIndices[slot];

	if(vertsAndColors.empty())
		return;

	for (size_t i = 0; i < vertsAndColors.size(); i += 7)
	{
		const vec3 pos(trans * dvec4(*(vec3*)&vertsAndColors[i], 1.0));

		memcpy(&vertsAndColors[i], &pos, sizeof(vec3));
	}

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	if(vertsAndColors.size())
	{
		glBufferData(GL_ARRAY_BUFFER, vertsAndColors.size() * sizeof(GLfloat), &vertsAndColors[0], GL_DYNAMIC_DRAW);
	}

	if(triangleIndices.size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), &triangleIndices[0], GL_DYNAMIC_DRAW);
	}

	if(lineIndices.size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * lineIndices.size(), &lineIndices[0], GL_DYNAMIC_DRAW);
	}

	const size_t sizeVertex = 3;
	const size_t sizeColor = 4;

	const size_t totalSize = (sizeVertex + sizeColor) * sizeof(GLfloat);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, sizeVertex, GL_FLOAT, GL_FALSE, totalSize, 0);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, sizeColor, GL_FLOAT, GL_FALSE, totalSize, (void*)((sizeVertex) * sizeof(GLfloat)));

	m_numTriIndices		= (GLuint)triangleIndices	.size();
	m_numLineIndices	= (GLuint)lineIndices		.size();

	vertsAndColors .clear();
	triangleIndices.clear();
	lineIndices    .clear();
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
    inline void addVert(const vec3 & v, const vec4 & color, const size_t slot)
    {
		auto & vertsAndColors = a_vertsAndColors[slot];

		vertsAndColors.push_back(float(v.x));
        vertsAndColors.push_back(float(v.y));
        vertsAndColors.push_back(float(v.z));

        vertsAndColors.push_back(float(color.x));
        vertsAndColors.push_back(float(color.y));
        vertsAndColors.push_back(float(color.z));
        vertsAndColors.push_back(float(color.w));
    }
}

void DeferredRenderable::addLine(const vec3		& A,
                                 const vec3		& B,
                                 const vec4		& color,
								 const size_t	  slot)
{
    const size_t index = a_vertsAndColors[slot].size() / 7;

    addVert(A, color, slot);
    addVert(B, color, slot);
    
    a_lineIndices[slot].push_back((uint32_t)index + 0);
    a_lineIndices[slot].push_back((uint32_t)index + 1);
}

void DeferredRenderable::addLine(const vec2		& A,
                                 const vec2		& B,
                                 const vec4		& color,
								 const size_t	  slot)
{
	addLine(vec3(A, 0), vec3(B, 0), color, slot);
}

void DeferredRenderable::addWireBox(const vec2		& min,
									const vec2		& max,
									const vec4		& color,
									const size_t	  slot)
{
	const auto P1 = vec2(min.x, max.y);
	const auto P2 = vec2(max.x, max.y);
	const auto P3 = vec2(max.x, min.y);
	const auto P4 = vec2(min.x, min.y);

	addLine(P1, P2, color, slot);
	addLine(P2, P3, color, slot);
	addLine(P3, P4, color, slot);
	addLine(P4, P1, color, slot);
}

void DeferredRenderable::addCrossHair(	const vec2		& pos,
										const float		  size,
										const vec4		& color,
										const size_t	  slot)
{
	addLine(pos - vec2(size, 0), pos + vec2(size, 0), color, slot);
	addLine(pos - vec2(0, size), pos + vec2(0, size), color, slot);
}

void DeferredRenderable::addCrossHairs(	const vector<vec2>	& positions,
										const float			  size,
										const glm::vec4		& color,
										const size_t		  slot)
{
	for(const auto & i : positions) { addCrossHair(i, size, color, slot); }
}

void DeferredRenderable::addTriangle(const vec3		& A,
                                     const vec3		& B,
                                     const vec3		& C,
                                     const vec4		& color,
									 const size_t	  slot)
{
    const size_t index = a_vertsAndColors[slot].size() / 7;

    addVert(A, color, slot);
    addVert(B, color, slot);
    addVert(C, color, slot);

    a_triangleIndices[slot].push_back((uint32_t)index + 0);
    a_triangleIndices[slot].push_back((uint32_t)index + 1);
    a_triangleIndices[slot].push_back((uint32_t)index + 2);
}

void DeferredRenderable::addTriangleWire(const vec3		& A,
										const vec3		& B,
										const vec3		& C,
										const vec4		& color,
										const size_t	  slot)
{
	addLine(A, B, color, slot);
	addLine(B, C, color, slot);
	addLine(C, A, color, slot);
}

void DeferredRenderable::addTriangle(const vec2		& A,
                                     const vec2		& B,
                                     const vec2		& C,
                                     const vec4		& color,
									 const size_t	  slot)
{
	addTriangle(vec3(A,0), vec3(B,0), vec3(C,0), color, slot);
}

void DeferredRenderable::addQuadrangle( const vec3		& A,
                                        const vec3		& B,
                                        const vec3		& C,
                                        const vec3		& D,
                                        const vec4		& color,
										const size_t	  slot)
{
    addVert(A, color, slot);
    addVert(B, color, slot);
    addVert(C, color, slot);
    addVert(A, color, slot);
    addVert(C, color, slot);
    addVert(D, color, slot);

	const size_t index = a_vertsAndColors[slot].size() / 7;

    a_triangleIndices[slot].push_back((uint32_t)index + 0);
    a_triangleIndices[slot].push_back((uint32_t)index + 1);
    a_triangleIndices[slot].push_back((uint32_t)index + 2);
    a_triangleIndices[slot].push_back((uint32_t)index + 3);
    a_triangleIndices[slot].push_back((uint32_t)index + 4);
    a_triangleIndices[slot].push_back((uint32_t)index + 5);
}

void DeferredRenderable::addCircle(	const vec2		& pos,
									const float		  radius,
									const vec4		& color,
									const size_t	  slot,
									const size_t	  numPoints)
{
	auto fullCircle = radians(360.0);
	
	const auto step = fullCircle / double(numPoints);
	
	fullCircle += step;

	for(double i = 0; i < fullCircle ;)
	{
		const auto A = pos + (vec2(cos(i), sin(i)) * vec2(radius, radius));

		i += step;

		const auto B = pos + (vec2(cos(i), sin(i)) * vec2(radius, radius));

		addLine(A, B, color, slot);
	}
}

void DeferredRenderable::addCircleFilled(	const vec2		& pos,
											const float		  radius,
											const vec4		& color,
											const size_t	  slot,
											const size_t	  numPoints)
{
	auto fullCircle = radians(360.0);
	
	const auto step = fullCircle / double(numPoints);
	
	fullCircle += step;

	for(double i = 0; i < fullCircle ;)
	{
		const auto A = pos + (vec2(cos(i), sin(i)) * vec2(radius, radius));

		i += step;

		const auto B = pos + (vec2(cos(i), sin(i)) * vec2(radius, radius));

		addTriangle(A, B, pos, color, slot);
	}
}

void DeferredRenderable::render(Canvas * canvas, const size_t renderStage)
{
    if(!m_shader->m_shouldRender(false, renderStage)) { return ;}

	m_shader->bind(canvas, false, renderStage);

    render();
}

void DeferredRenderable::render(const mat4		& model,
								const mat4		& view,
								const mat4		& projection,
								const size_t	  renderStage)
{
	if(!m_shader->m_shouldRender(false, renderStage)) { return ;}

	m_shader->bind(model, view, projection, false, renderStage);

    render();
}

void DeferredRenderable::render() const
{
	glBindVertexArray(					  m_vao);
    glBindBuffer(GL_ARRAY_BUFFER,         m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

    glDrawElements(GL_TRIANGLES, m_numTriIndices, GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);

    glDrawElements(GL_LINES, m_numLineIndices, GL_UNSIGNED_INT, nullptr);
}