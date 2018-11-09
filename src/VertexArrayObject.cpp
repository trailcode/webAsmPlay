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
#include <webAsmPlay/VertexArrayObject.h>

using namespace std;
using namespace glm;

VertexArrayObject * VertexArrayObject::create(const Tessellations & tessellations)
{
    if(tessellations[0]->getHeight() != 0.0) { return _create<true>(tessellations) ;}

    return _create<false>(tessellations);
}

template<bool IS_3D>
VertexArrayObject * VertexArrayObject::_create(const Tessellations & tessellations)
{
    if(!tessellations.size()) { return NULL ;}

    size_t numVerts               = 0;
    size_t numTriangles           = 0;
    size_t numCounterVertIndices  = 0;
    size_t numlineIndices         = 0;

    for(const auto & tess : tessellations)
    {
        numVerts               += tess->numVerts;
        numTriangles           += tess->numTriangles;
        numCounterVertIndices  += tess->counterVertIndices.size();

        if(IS_3D) { numlineIndices += tess->lineIndices.size() * 3 ;}
        else      { numlineIndices += tess->lineIndices.size()     ;}
    }

    FloatVec verts;

    if(IS_3D) { verts.resize(numVerts * (3 + 3 + 1 + 1) * 2) ;}
    else      { verts.resize(numVerts * (2 + 1)) ;}

    Uint32Vec triangleIndices      (numTriangles * 3);
    Uint32Vec counterVertIndices   (numCounterVertIndices);
    Uint32Vec lineIndices          (numlineIndices);

    GLfloat * vertsPtr               = &verts[0];
    GLuint  * triangleIndicesPtr     = &triangleIndices[0];
    GLuint  * counterVertIndicesPtr  = &counterVertIndices[0];
    GLuint  * lineIndicesPtr         = &lineIndices[0];

    size_t offset = 0;

    vector<size_t> wallTris;

    for(size_t i = 0; i < tessellations.size(); ++i)
    {
        const auto & tess = tessellations[i];

        const float symbologyID_value = (float(tess->symbologyID * 6) + 0.5) / 32.0;

        const float symbologyWallID_value = (float(tess->symbologyID * 6) + 0.5) / 32.0 + 4.0 / 32.0;

        const size_t startIndex = *counterVertIndicesPtr;

        //const vec3 normal;

        for(size_t j = 0; j < tess->numVerts; ++j)
        {
            append(vertsPtr, tess->verts[j * 2 + 0]);
            append(vertsPtr, tess->verts[j * 2 + 1]);

            if(IS_3D)
            {
                append(vertsPtr, tess->height);

                append(vertsPtr, 0);
                append(vertsPtr, 0);
                append(vertsPtr, 1);
            }

            append(vertsPtr, symbologyID_value);
        }

        for(size_t j = 0; j < tess->numTriangles * 3; ++j, ++triangleIndicesPtr) { *triangleIndicesPtr = tess->triangleIndices[j] + offset ;}

        for(size_t j = 0; j < tess->counterVertIndices.size(); ++j, ++counterVertIndicesPtr) { *counterVertIndicesPtr = tess->counterVertIndices[j] + offset ;}

        for(size_t j = 0; j < tess->lineIndices.size(); ++j, ++lineIndicesPtr) { *lineIndicesPtr = tess->lineIndices[j] + offset ;}

        offset += tess->numVerts;

        if(IS_3D && tess->verts)
        {
            size_t prevIndexA = append(lineIndicesPtr, (vertsPtr - &verts[0]) / 7);
            
            const vec3 A(tess->verts[0], tess->verts[1], 0);
            const vec3 B(tess->verts[0], tess->verts[1], tess->height);
            const vec3 C(tess->verts[2], tess->verts[3], 0);

            const vec3 normal = triangleNormal(A, B, C);

            append(vertsPtr, A);
            append(vertsPtr, normal);
            append(vertsPtr, symbologyWallID_value);

            size_t prevIndexB = append(lineIndicesPtr, (vertsPtr - &verts[0]) / 7);

            const size_t startIndexA = prevIndexA;
            const size_t startIndexB = prevIndexB;

            append(vertsPtr, B);
            append(vertsPtr, normal);
            append(vertsPtr, symbologyWallID_value);

            for(size_t j = 1; j < tess->numVerts; ++j)
            {
                const size_t indexA = append(lineIndicesPtr, (vertsPtr - &verts[0]) / 7);

                const size_t k = (j + 1) % tess->numVerts;

                const vec3 A(tess->verts[j * 2 + 0], tess->verts[j * 2 + 1], 0);
                const vec3 B(tess->verts[j * 2 + 0], tess->verts[j * 2 + 1], tess->height);
                const vec3 C(tess->verts[k * 2 + 0], tess->verts[k * 2 + 1], 0);

                const vec3 normal = triangleNormal(A, B, C);

                append(vertsPtr, A);
                
                append(vertsPtr, normal);
                
                append(vertsPtr, symbologyWallID_value);

                const size_t indexB = append(lineIndicesPtr, (vertsPtr - &verts[0]) / 7);

                append(vertsPtr, B);

                append(vertsPtr, normal);

                append(vertsPtr, symbologyWallID_value);

                wallTris.push_back(prevIndexA);
                wallTris.push_back(prevIndexB);
                wallTris.push_back(indexA);

                wallTris.push_back(indexA);
                wallTris.push_back(prevIndexB);
                wallTris.push_back(indexB);
                
                prevIndexA = indexA;
                prevIndexB = indexB;
            }

            wallTris.push_back(startIndexA);
            wallTris.push_back(startIndexB);
            wallTris.push_back(prevIndexA);

            wallTris.push_back(prevIndexA);
            wallTris.push_back(startIndexB);
            wallTris.push_back(prevIndexB);

            //dmess("tess->numVerts " << tess->numVerts << " wallTris " << wallTris.size());

            offset += tess->numVerts * 2;
        }
    }

    triangleIndices.insert(triangleIndices.begin(), wallTris.begin(), wallTris.end()); // TODO try not to do this.

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
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * lineIndices.size(), &lineIndices[0], GL_STATIC_DRAW));

    return new VertexArrayObject(vao,
                                 ebo,
                                 ebo2,
                                 vbo,
                                 triangleIndices.size(),
                                 counterVertIndices,
                                 lineIndices.size(),
                                 tessellations.size() > 1);
}

VertexArrayObject::VertexArrayObject(   const GLuint      vao,
                                        const GLuint      ebo,
                                        const GLuint      ebo2,
                                        const GLuint      vbo,
                                        const GLuint      numTrianglesIndices,
                                        const Uint32Vec & counterVertIndices,
                                        const size_t      numContourLines,
                                        const bool        isMulti) : vao                (vao),
                                                                     ebo                (ebo),
                                                                     ebo2               (ebo2),
                                                                     vbo                (vbo),
                                                                     numTrianglesIndices(numTrianglesIndices),
                                                                     counterVertIndices (counterVertIndices),
                                                                     numContourLines    (numContourLines),
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
    GL_CHECK(glDrawElements(GL_TRIANGLES, numTrianglesIndices, GL_UNSIGNED_INT, NULL));
}

void VertexArrayObject::drawLines() const
{
    GL_CHECK(glDrawElements(GL_LINES, numContourLines, GL_UNSIGNED_INT, NULL));
}

bool VertexArrayObject::isMulti() const { return _isMulti ;}