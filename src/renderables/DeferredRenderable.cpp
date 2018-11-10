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
#include <vector>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>

using namespace std;
using namespace glm;

namespace
{
    vector<GLfloat> vertsAndColor;
    vector<GLuint>  triangleIndices;
    vector<GLuint>  lineIndices;

    inline void addTriangle(const vec3  & A,
                            const vec3  & B,
                            const vec3  & C,
                            const vec4  & color,
                            size_t      & startIndex,
                            GLfloat    *& vertsAndColorPtr,
                            GLuint     *& triangleIndicesPtr)
    {
        memcpy(vertsAndColorPtr, value_ptr(A), sizeof(GLfloat) * 3); vertsAndColorPtr += 3;
        memcpy(vertsAndColorPtr, value_ptr(B), sizeof(GLfloat) * 3); vertsAndColorPtr += 3;
        memcpy(vertsAndColorPtr, value_ptr(C), sizeof(GLfloat) * 3); vertsAndColorPtr += 3;

        memcpy(vertsAndColorPtr, value_ptr(color), sizeof(GLfloat) * 4); vertsAndColorPtr += 4;

        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
    }
}

DeferredRenderable * DeferredRenderable::createFromQueued()
{
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

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertsAndColor.size() * sizeof(GLfloat), &vertsAndColor[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * lineIndices.size(), &lineIndices[0], GL_STATIC_DRAW));

    DeferredRenderable * ret = new DeferredRenderable(vao, ebo, ebo2, vbo, triangleIndices.size(), lineIndices.size());

    vertsAndColor.clear();
    triangleIndices.clear();
    lineIndices.clear();

    return ret;
}

DeferredRenderable::DeferredRenderable( const GLuint & vao,
                                        const GLuint & ebo,
                                        const GLuint & ebo2,
                                        const GLuint & vbo,
                                        const GLuint & numTriIndices,
                                        const GLuint & numLineIndices) : Renderable    (false),
                                                                         vao           (vao),
                                                                         ebo           (ebo),
                                                                         ebo2          (ebo2),
                                                                         vbo           (vbo),
                                                                         numTriIndices (numTriIndices),
                                                                         numLineIndices(numLineIndices)
{

}

DeferredRenderable::~DeferredRenderable()
{
    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers     (1, &vbo));
    GL_CHECK(glDeleteBuffers     (1, &ebo));
    GL_CHECK(glDeleteBuffers     (1, &ebo2));
}

namespace
{
    inline void addVert(const vec3 & v, const vec4 & color)
    {
        vertsAndColor.push_back(v.x * 0.1);
        vertsAndColor.push_back(v.y * 0.1);
        vertsAndColor.push_back(v.z * 0.1);
        vertsAndColor.push_back(color.x);
        vertsAndColor.push_back(color.y);
        vertsAndColor.push_back(color.z);
        vertsAndColor.push_back(color.w);
    }
}

void DeferredRenderable::addLine(const vec3 & A,
                                 const vec3 & B,
                                 const vec4 & color)
{
    const size_t index = vertsAndColor.size() / 7;

    addVert(A, color);
    addVert(B, color);
    addVert(B, color);

    lineIndices.push_back(index + 0);
    lineIndices.push_back(index + 1);
}

void DeferredRenderable::addTriangle(const vec3 & A,
                                     const vec3 & B,
                                     const vec3 & C,
                                     const vec4 & color)
{
    const size_t index = vertsAndColor.size() / 7;

    addVert(A, color);
    addVert(B, color);
    addVert(C, color);

    triangleIndices.push_back(index + 0);
    triangleIndices.push_back(index + 1);
    triangleIndices.push_back(index + 2);
}

void DeferredRenderable::addQuadrangle( const vec3 & A,
                                        const vec3 & B,
                                        const vec3 & C,
                                        const vec3 & D,
                                        const vec4 & color)
{
    const size_t index = vertsAndColor.size() / 7;

    addVert(A, color);
    addVert(B, color);
    addVert(C, color);
    addVert(A, color);
    addVert(C, color);
    addVert(D, color);

    triangleIndices.push_back(index + 0);
    triangleIndices.push_back(index + 1);
    triangleIndices.push_back(index + 2);
    triangleIndices.push_back(index + 3);
    triangleIndices.push_back(index + 4);
    triangleIndices.push_back(index + 5);

    /*
    size_t curr = vertsAndColor.size();

    vertsAndColor.resize(curr + ((3 + 4) * 6));

    size_t currIndex = curr / 7;

    triangleIndices.resize(triangleIndices.size() + 6);

    GLfloat * vertsAndColorPtr = &vertsAndColor[curr];
    GLuint  * triangleIndicesPtr        = &triangleIndices[currIndex];

    //dmess("currIndex " << currIndex);

    addTriangle(A,
                B,
                C,
                color,
                currIndex,
                vertsAndColorPtr,
                triangleIndicesPtr);
    
    addTriangle(A,
                C,
                D,
                color,
                currIndex,
                vertsAndColorPtr,
                triangleIndicesPtr);
    */


    //dmess("currIndex " << currIndex);
}

void DeferredRenderable::render(const mat4 & model,
                                const mat4 & view,
                                const mat4 & projection) const
{
    GL_CHECK(glBindVertexArray(vao));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER,         vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

    const mat4 _model = glm::rotate(model, radians(90.0f), vec3(1, 0, 0)); // TODO might not be correct, might need to define model first!

    ColorVertexShader::getInstance()->bind(_model, view, projection, false);

    ColorVertexShader::getInstance()->enableVertexAttribArray(3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    ColorVertexShader::getInstance()->enableColorAttribArray(4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GL_FLOAT)));

    GL_CHECK(glDrawElements(GL_TRIANGLES, numTriIndices, GL_UNSIGNED_INT, NULL));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));

    GL_CHECK(glDrawElements(GL_LINES, numLineIndices, GL_UNSIGNED_INT, NULL));
}