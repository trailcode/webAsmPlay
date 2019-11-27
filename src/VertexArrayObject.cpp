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

#include <glm/gtx/normal.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/VertexArrayObject.h>

using namespace std;
using namespace glm;

VertexArrayObject * VertexArrayObject::create(const Tessellations & tessellations)
{
    if(tessellations[0]->getHeight() != 0.0)
    {
        return _create< true,  // 3D extrude
                        true,  // Use symbology ID
                        false, // Use UV coords
						false  // Swap UV Axis
                      > (tessellations, AABB2D());
    }

    return _create< false, // 3D extrude
                    true,  // Use symbology ID
                    false, // Use UV coords
					false  // Swap UV Axis
                  > (tessellations, AABB2D());
}

VertexArrayObject * VertexArrayObject::create(const Tessellations & tessellations, const AABB2D & boxUV, const bool swapUV_Axis)
{
	if(swapUV_Axis)
	{
		return _create< false, // 3D extrude
						false, // Use symbology ID
						true,  // Use UV coords
						true   // Swap UV Axis
						> (tessellations, boxUV);
	}
	else
	{
		return _create< false, // 3D extrude
						false, // Use symbology ID
						true,  // Use UV coords
						false  // Swap UV Axis
						> (tessellations, boxUV);
	}
}

namespace
{
    template <bool USE_SYMBOLOGY_ID>
    void addVert(   FloatVec    & verts,
                    const vec3  & v,
                    const vec3  & n,
                    const float   c)
    {
        verts.push_back(v.x);
        verts.push_back(v.y);
        verts.push_back(v.z);

        verts.push_back(n.x);
        verts.push_back(n.y);
        verts.push_back(n.z);

        if(USE_SYMBOLOGY_ID) { verts.push_back(c) ;}
    }
}

template<bool IS_3D, bool USE_SYMBOLOGY_ID, bool USE_UV_COORDS, bool SWAP_UV_AXIS>
VertexArrayObject * VertexArrayObject::_create(const Tessellations & tessellations, const AABB2D & boxUV)
{
    if(!tessellations.size()) { return nullptr ;}

    FloatVec  verts;
    Uint32Vec triangleIndices;
    Uint32Vec lineIndices;

    size_t offset = 0;

    size_t symbologyID_Stride;

    if(IS_3D) { symbologyID_Stride = 8 ;}
    else      { symbologyID_Stride = 4 ;}

    for(const auto & tess : tessellations)
    {
        // TODO try to remove hard coded values.
        const float symbologyID_value     = float((float(tess->m_symbologyID * symbologyID_Stride) + 0.5) / 32.0);
        const float symbologyWallID_value = float((float(tess->m_symbologyID * symbologyID_Stride) + 0.5) / 32.0 + 4.0 / 32.0);

        for(size_t i = 0; i < tess->m_numVerts; ++i)
        {
            const dvec2 P(tess->m_verts[i * 2 + 0], tess->m_verts[i * 2 + 1]);

            verts.push_back((float)P.x);
            verts.push_back((float)P.y);

            if(IS_3D)  
            {
                verts.push_back((float)tess->m_height);

                verts.push_back(0);
                verts.push_back(0);
                verts.push_back(1);
            }

            if(USE_SYMBOLOGY_ID) { verts.push_back(symbologyID_value) ;}

            if(USE_UV_COORDS)
            {
				const dvec2 min(get<0>(boxUV), get<1>(boxUV));
                const dvec2 max(get<2>(boxUV), get<3>(boxUV));

                const dvec2 uv = (P - min) / (max - min);

				if(SWAP_UV_AXIS)
				{
					verts.push_back(float(uv.x));
					verts.push_back(float(1 - uv.y));
				}
				else
				{
					verts.push_back(float(uv.y));
					verts.push_back(float(1 - uv.x));
				}
            }
        }

        for(size_t i = 0; i < tess->m_numTriangles * 3; ++i) { triangleIndices.push_back(uint32_t(tess->m_triangleIndices[i] + offset)) ;}

        size_t lastIndex = 0;

        for(size_t i = 1; i < tess->m_counterVertIndices.size(); ++i)
        {
            const size_t num = tess->m_counterVertIndices[i] - lastIndex;

            const size_t localOffset = offset + lastIndex;

            for(size_t j = 0; j < num; ++j)
            {
                lineIndices.push_back(uint32_t(j + localOffset));
                lineIndices.push_back(uint32_t((j + 1) % num + localOffset));
            }

            lastIndex = tess->m_counterVertIndices[i];
        }

        offset += tess->m_numVerts;

        if(!IS_3D) { continue ;}

        for(size_t a = 0; a < tess->m_numVerts; ++a)
        {
            const size_t b = (a + 1) % tess->m_numVerts;

            const vec2 A(tess->m_verts[a * 2], tess->m_verts[a * 2 + 1]);
            const vec2 B(tess->m_verts[b * 2], tess->m_verts[b * 2 + 1]);

            const vec3 p1(A, tess->m_height);
            const vec3 p2(B, tess->m_height);
            const vec3 p3(B, tess->m_minHeight);
            const vec3 p4(A, tess->m_minHeight);

			const vec3 normal = -normalize(triangleNormal(p1, p2, p3)); // Why is this backwards on different system? Winding wrong.

            addVert<USE_SYMBOLOGY_ID>(verts, p1, normal, symbologyWallID_value);
            addVert<USE_SYMBOLOGY_ID>(verts, p2, normal, symbologyWallID_value);
            addVert<USE_SYMBOLOGY_ID>(verts, p3, normal, symbologyWallID_value);
            addVert<USE_SYMBOLOGY_ID>(verts, p4, normal, symbologyWallID_value);

            triangleIndices.push_back((uint32_t)offset + 0);
            triangleIndices.push_back((uint32_t)offset + 1);
            triangleIndices.push_back((uint32_t)offset + 2);

            triangleIndices.push_back((uint32_t)offset + 2);
            triangleIndices.push_back((uint32_t)offset + 3);
            triangleIndices.push_back((uint32_t)offset);

            lineIndices.push_back((uint32_t)offset + 0);
            lineIndices.push_back((uint32_t)offset + 3);

            offset += 4;
        }
    }

	if (!triangleIndices.size())
	{
		dmess("Warning !triangleIndices.size()");

		return nullptr;
	}

    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &ebo2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * lineIndices.size(), &lineIndices[0], GL_STATIC_DRAW);

	// Flush is required if executing in a thread different from the main thread.
	glFlush();

    size_t sizeVertex = 2;
    size_t sizeColor  = 0;
    size_t sizeNormal = 0;
    size_t sizeUV     = 0;

    if(IS_3D)
    {
        sizeVertex = 3;
        sizeNormal = 3;
    }

    if(USE_SYMBOLOGY_ID) { sizeColor = 1 ;}
    if(USE_UV_COORDS)	 { sizeUV	 = 2 ;}

    return new VertexArrayObject(	ebo,
									ebo2,
									vbo,
									triangleIndices.size(),
									lineIndices.size(),
									tessellations.size() > 1,
									sizeVertex,
									sizeNormal,
									sizeColor,
									sizeUV);
}

VertexArrayObject::VertexArrayObject(   const GLuint ebo,
                                        const GLuint ebo2,
                                        const GLuint vbo,
                                        const GLuint numTrianglesIndices,
                                        const size_t numContourLines,
                                        const bool   isMulti,
										const size_t sizeVertex,
										const size_t sizeNormal,
										const size_t sizeColor,
										const size_t sizeUV) :	m_ebo					(ebo),
																m_ebo2					(ebo2),
																m_vbo					(vbo),
																m_numTrianglesIndices	(numTrianglesIndices),
																m_numContourLines		(numContourLines),
																m_isMulti				(isMulti),
																m_sizeVertex			(sizeVertex),
																m_sizeNormal			(sizeNormal),
																m_sizeColor				(sizeColor),
																m_sizeUV				(sizeUV)
{

}

VertexArrayObject::~VertexArrayObject()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers     (1, &m_vbo);
    glDeleteBuffers     (1, &m_ebo);
    glDeleteBuffers     (1, &m_ebo2);
}

void VertexArrayObject::bind(Shader * shader) const
{
    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void VertexArrayObject::bindTriangles() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

void VertexArrayObject::bindLines() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
}

void VertexArrayObject::drawTriangles() const
{
    glDrawElements(GL_TRIANGLES, m_numTrianglesIndices, GL_UNSIGNED_INT, nullptr);
}

void VertexArrayObject::drawLines() const
{
    glDrawElements(GL_LINES, (GLsizei)m_numContourLines, GL_UNSIGNED_INT, nullptr);
}

bool VertexArrayObject::isMulti() const { return m_isMulti ;}

void VertexArrayObject::ensureVAO()
{
	if(m_vao) { return ;}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(    m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	
	const size_t totalSize = (m_sizeVertex + m_sizeColor + m_sizeNormal + m_sizeUV) * sizeof(GLfloat);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, (GLint)m_sizeVertex, GL_FLOAT, GL_FALSE, (GLsizei)totalSize, 0);

	if(m_sizeColor)
	{
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(1, (GLint)m_sizeColor, GL_FLOAT, GL_FALSE, (GLsizei)totalSize, (void *)((m_sizeVertex + m_sizeNormal) * sizeof(GLfloat)));
	}

	if(m_sizeNormal)
	{
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(2, (GLint)m_sizeNormal, GL_FLOAT, GL_FALSE, (GLsizei)totalSize, (void *)(m_sizeVertex * sizeof(GLfloat)));
	}

	if(m_sizeUV)
	{
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(3, (GLint)m_sizeUV, GL_FLOAT, GL_FALSE, (GLsizei)totalSize, (void *)((m_sizeVertex + m_sizeNormal + m_sizeColor) * sizeof(GLfloat)));
	}

	glBindVertexArray(0);
}

size_t VertexArrayObject::getNumTriangles() const { return m_numTrianglesIndices / 3 ;}