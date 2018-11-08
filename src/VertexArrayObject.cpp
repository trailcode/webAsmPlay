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
#include <webAsmPlay/VertexArrayObject.h>

VertexArrayObject * VertexArrayObject::create(const Tessellations & tessellations)
{
    return _create<false>(tessellations);
}

template<bool IS_3D>
VertexArrayObject * VertexArrayObject::_create(const Tessellations & tessellations)
{
    if(!tessellations.size()) { return NULL ;}

    size_t numVerts                 = 0;
    size_t numTriangles             = 0;
    size_t numCounterVertIndices    = 0;
    size_t numCounterVertIndices2   = 0;

    for(const Tessellation & tess : tessellations)
    {
        numVerts                += tess.numVerts;
        numTriangles            += tess.numTriangles;
        numCounterVertIndices   += tess.counterVertIndices.size();
        numCounterVertIndices2  += tess.counterVertIndices2.size();
    }

    FloatVec verts;

    verts.resize(numVerts * (2 + 1));

    Uint32Vec triangleIndices      (numTriangles * 3);
    Uint32Vec counterVertIndices   (numCounterVertIndices);
    Uint32Vec counterVertIndices2  (numCounterVertIndices2);

    GLfloat * vertsPtr               = &verts[0];
    GLuint  * triangleIndicesPtr     = &triangleIndices[0];
    GLuint  * counterVertIndicesPtr  = &counterVertIndices[0];
    GLuint  * counterVertIndicesPtr2 = &counterVertIndices2[0];

    size_t offset = 0;

    for(size_t i = 0; i < tessellations.size(); ++i)
    {
        const Tessellation & tess = tessellations[i];

        for(size_t j = 0; j < tess.numVerts; ++j)
        {
            *vertsPtr = tess.vertsOut[j * 2 + 0]; ++vertsPtr;
            *vertsPtr = tess.vertsOut[j * 2 + 1]; ++vertsPtr;

            *vertsPtr = (float(tessellations[i].symbologyID * 4) + 0.5) / 32.0; ++vertsPtr;
        }

        for(size_t j = 0; j < tess.numTriangles * 3; ++j, ++triangleIndicesPtr) { *triangleIndicesPtr = tess.triangleIndices[j] + offset ;}

        for(size_t j = 0; j < tess.counterVertIndices.size(); ++j, ++counterVertIndicesPtr) { *counterVertIndicesPtr = tess.counterVertIndices[j] + offset ;}

        for(size_t j = 0; j < tess.counterVertIndices2.size(); ++j, ++counterVertIndicesPtr2) { *counterVertIndicesPtr2 = tess.counterVertIndices2[j] + offset ;}

        offset += tess.numVerts;

        free(tess.vertsOut);
        free(tess.triangleIndices);
    }

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glGenBuffers(1, &ebo));
    GL_CHECK(glGenBuffers(1, &ebo2));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, &triangleIndices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * counterVertIndices2.size(), &counterVertIndices2[0], GL_STATIC_DRAW));

    return new VertexArrayObject(vao,
                                 ebo,
                                 ebo2,
                                 vbo,
                                 numTriangles,
                                 counterVertIndices,
                                 counterVertIndices2.size(),
                                 tessellations.size() > 1);
}

VertexArrayObject::VertexArrayObject(   const GLuint      vao,
                                        const GLuint      ebo,
                                        const GLuint      ebo2,
                                        const GLuint      vbo,
                                        const int         numTriangles,
                                        const Uint32Vec & counterVertIndices,
                                        const size_t      numContourLines,
                                        const bool        isMulti) : vao                (vao),
                                                                     ebo                (ebo),
                                                                     ebo2               (ebo2),
                                                                     vbo                (vbo),
                                                                     numTriangles       (numTriangles),
                                                                     counterVertIndices (counterVertIndices),
                                                                    numContourLines     (numContourLines),
                                                                     _isMulti           (isMulti)
{

}

VertexArrayObject::~VertexArrayObject()
{
    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers     (1, &vbo));
    GL_CHECK(glDeleteBuffers     (1, &ebo));
    GL_CHECK(glDeleteBuffers     (1, &ebo2));
}

void VertexArrayObject::bind() const
{
    GL_CHECK(glBindVertexArray(vao));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
}

void VertexArrayObject::bindTriangles() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
}

void VertexArrayObject::bindLines() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
}

void VertexArrayObject::drawTriangles() const
{
    GL_CHECK(glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, NULL));
}

void VertexArrayObject::drawLines() const
{
    GL_CHECK(glDrawElements(GL_LINES, numContourLines, GL_UNSIGNED_INT, NULL));
}

bool VertexArrayObject::isMulti() const { return _isMulti ;}