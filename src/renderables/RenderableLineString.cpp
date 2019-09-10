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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/renderables/RenderableLineString.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

RenderableLineString::RenderableLineString( const GLuint  ebo,
                                            const GLuint  vbo,
                                            const size_t  numElements,
                                            const bool    isMulti) :    Renderable(   isMulti,
                                                                                      false,
                                                                                      GUI::s_renderSettingsRenderPolygonOutlines),
                                                                        m_ebo        (ebo),
                                                                        m_vbo        (vbo),
                                                                        m_numElements(numElements)
{
}

RenderableLineString::~RenderableLineString()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers     (1, &m_ebo);
    glDeleteBuffers     (1, &m_vbo);
}

Renderable * RenderableLineString::create(  const LineString * lineString,
                                            const dmat4      & trans)
{
    if(!lineString)
    {
        dmess("Error lineString is NULL!");

        return nullptr;
    }

    return create(*lineString->getCoordinatesRO()->toVector(), trans);
}

Renderable * RenderableLineString::create(const vector<Coordinate> & coords, const dmat4 & trans)
{
    if(coords.size() < 2)
    {
        dmess("Bad geometry!");

        return nullptr;
    }

    FloatVec  verts  (coords.size() * 2);
    Uint32Vec indices(coords.size());

    GLfloat * vertsPtr = &verts[0];

    if(trans == dmat4(1.0))
    {
        for(size_t i = 0; i < coords.size(); ++i)
        {
            append2f(vertsPtr, coords[i]);

            indices[i] = (uint32_t)i;
        }
    }
    else
    {
        for(size_t i = 0; i < coords.size(); ++i)
        {
            append2f(vertsPtr, trans * dvec4(coords[i].x, coords[i].y, 0, 1));

            indices[i] = (uint32_t)i;
        }
    }

    return create(  verts,
                    indices,
                    false // isMulti
                );
}

Renderable * RenderableLineString::create(const ColoredGeometryVec & lineStrings,
                                          const dmat4              & trans,
                                          const bool                 showProgress)
{
    time_point<system_clock> startTime;
    
    if(showProgress) { startTime = system_clock::now() ;}

    size_t numVerts = 0;

    for(const auto & ls : lineStrings) { numVerts += dynamic_cast<const LineString *>(get<0>(ls))->getNumPoints() ;}

    FloatVec  verts(numVerts * 3);

    Uint32Vec indices; // TODO try not to push_back

    GLfloat * vertsPtr = &verts[0];

    size_t index = 0;

    for(size_t i = 0; i < lineStrings.size(); ++i)
    {
        if(showProgress) { doProgress("(3/6) Creating geometry:", i, lineStrings.size(), startTime) ;}

        const Geometry  * geom        = get<0>(lineStrings[i]);
        const float       symbologyID = (float(get<1>(lineStrings[i]) * 4) + 0.5f) / 32.0f;

        const vector<Coordinate> & coords = *dynamic_cast<const LineString *>(geom)->getCoordinatesRO()->toVector();

		if(trans == dmat4(1.0)) { dmessError("Implement!") ;}
        else
        {
            append2f(vertsPtr, trans * dvec4(coords[0].x, coords[0].y, 0, 1));

            append(vertsPtr, symbologyID);
            
            indices.push_back((uint32_t)index++);

            for(size_t i = 1; i < coords.size() - 1; ++i)
            {
                append2f(vertsPtr, trans * dvec4(coords[i].x, coords[i].y, 0, 1));
                
                append(vertsPtr, symbologyID);

                indices.push_back((uint32_t)index);
                indices.push_back((uint32_t)index);
                
                ++index;
            }

            append2f(vertsPtr, trans * dvec4(coords.rbegin()->x, coords.rbegin()->y, 0, 1));

            append(vertsPtr, symbologyID);

            indices.push_back((uint32_t)index++);
        }
    }

    Renderable * ret = create(verts, indices, true);

    if(showProgress) { GUI::progress("", 1.0) ;}

    return ret;
}

Renderable * RenderableLineString::create(  const FloatVec  & verts,
                                            const Uint32Vec & indices,
                                            const bool        isMulti)
{
    GLuint ebo = 0;
    GLuint vbo = 0;

    glGenBuffers     (1, &ebo);
    glGenBuffers     (1, &vbo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), &verts[0], GL_STATIC_DRAW);

	// Flush is required if executing in a thread different from the main thread.
	glFlush();
    
	// TODO use the VertexArrayObject
    return new RenderableLineString(ebo,
                                    vbo,
                                    indices.size(),
                                    isMulti);
}

void RenderableLineString::ensureVAO()
{
	if(m_vao) { return ;}

	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	if(!m_isMulti)
	{
		const size_t totalSize = 2 * sizeof(GLfloat);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, totalSize, 0);
	}
	else
	{
		const size_t totalSize = (2 + 1) * sizeof(GLfloat);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, totalSize, 0);

		glEnableVertexAttribArray(1);

		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, totalSize, (void *)((2) * sizeof(GLfloat)));
	}
}

void RenderableLineString::render(Canvas * canvas, const size_t renderStage)
{
    if(!m_shader->m_shouldRender(true, renderStage)) { return ;}

    if(!getRenderOutline()) { return ;}

    glBindVertexArray(                    m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	glLineWidth(GUI::s_lineWidthRender);

    if(!m_isMulti)
    {
        m_shader->bind(canvas, true, renderStage);

        glDrawElements(GL_LINE_STRIP, (GLsizei)m_numElements, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        m_shader->bind(canvas, true, renderStage);

        glDrawElements(GL_LINES, (GLsizei)m_numElements, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
}

