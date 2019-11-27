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

#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/geom/Tessellation.h>

class Shader;

class VertexArrayObject
{
public:

    static VertexArrayObject * create(const Tessellations & tessellations);

    static VertexArrayObject * create(const Tessellations & tessellations, const AABB2D & boxUV, const bool swapUV_Axis = false);

    ~VertexArrayObject();

    void bind(Shader * shader) const;

    void bindTriangles() const;

    void bindLines() const;

    void drawTriangles() const;

    void drawLines() const;

    bool isMulti() const;

	void ensureVAO();

	size_t getNumTriangles() const;

private:

    template<bool IS_3D, bool USE_SYMBOLOGY_ID, bool USE_UV_COORDS, bool SWAP_UV_AXIS>
    static VertexArrayObject * _create(const Tessellations & tessellations, const AABB2D & boxUV);

    VertexArrayObject(  const GLuint        ebo,
                        const GLuint        ebo2,
                        const GLuint        vbo,
                        const GLuint        numTrianglesIndices,
                        const size_t        numContourLines,
                        const bool          isMulti,
						const size_t		sizeVertex,
						const size_t		sizeNormal,
						const size_t		sizeColor,
						const size_t		sizeUV);

    VertexArrayObject(const VertexArrayObject &)              = delete;
    VertexArrayObject(VertexArrayObject &&)                   = delete;
    VertexArrayObject & operator=(const VertexArrayObject &)  = delete;
    
		  GLuint    m_vao                 = 0;
    const GLuint    m_ebo                 = 0;
    const GLuint    m_ebo2                = 0;
    const GLuint    m_vbo                 = 0;
    const GLuint    m_numTrianglesIndices = 0;
    const size_t    m_numContourLines     = 0;
    const bool      m_isMulti             = false;

	const size_t m_sizeVertex;
	const size_t m_sizeNormal;
	const size_t m_sizeColor;
	const size_t m_sizeUV;
};
