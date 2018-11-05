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
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/DeferredRenderable.h>

using namespace std;
using namespace glm;

namespace
{
    vector<GLfloat> triangleVertsAndColors;
    vector<GLuint>  triangleIndices;

    inline void addTriangle(const vec3    & A,
                            const vec3    & B,
                            const vec3    & C,
                            const vec4    & color,
                            size_t        & startIndex,
                            GLfloat      *& triangleVertsAndColorsPtr,
                            GLuint       *& triangleIndicesPtr)
    {
        memcpy(triangleVertsAndColorsPtr, value_ptr(A), sizeof(GLfloat) * 3); triangleVertsAndColorsPtr += 3;
        memcpy(triangleVertsAndColorsPtr, value_ptr(B), sizeof(GLfloat) * 3); triangleVertsAndColorsPtr += 3;
        memcpy(triangleVertsAndColorsPtr, value_ptr(C), sizeof(GLfloat) * 3); triangleVertsAndColorsPtr += 3;

        memcpy(triangleVertsAndColorsPtr, value_ptr(color), sizeof(GLfloat) * 4); triangleVertsAndColorsPtr += 4;

        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
    }
}

DeferredRenderable * DeferredRenderable::createFromQueued()
{
    dmess("triangleVertsAndColors " << triangleVertsAndColors.size());
    dmess("triangleIndices " << triangleIndices.size());

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint vbo  = 0;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, triangleVertsAndColors.size() * sizeof(GLfloat), &triangleVertsAndColors[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW);
    
    DeferredRenderable * ret = new DeferredRenderable(vao, ebo, vbo, triangleIndices.size());

    triangleVertsAndColors.clear();
    triangleIndices.clear();

    return ret;
}

DeferredRenderable::DeferredRenderable( const GLuint & vao,
                                        const GLuint & ebo,
                                        const GLuint & vbo,
                                        const GLuint & numTriIndices) : Renderable   (false),
                                                                        vao          (vao),
                                                                        ebo          (ebo),
                                                                        vbo          (vbo),
                                                                        numTriIndices(numTriIndices)
{

}

DeferredRenderable::~DeferredRenderable()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
    glDeleteBuffers     (1, &ebo);
}

namespace
{
    inline void addVert(const vec3 & v, const vec4 & color)
    {
        triangleVertsAndColors.push_back(v.x * 0.1);
        triangleVertsAndColors.push_back(v.y * 0.1);
        triangleVertsAndColors.push_back(v.z * 0.1);
        triangleVertsAndColors.push_back(color.x);
        triangleVertsAndColors.push_back(color.y);
        triangleVertsAndColors.push_back(color.z);
        triangleVertsAndColors.push_back(color.w);
    }
}

void DeferredRenderable::addTriangle(const vec3 & A,
                                     const vec3 & B,
                                     const vec3 & C,
                                     const vec4 & color)
{
    addVert(A, color);
    addVert(B, color);
    addVert(C, color);

    const size_t index = triangleVertsAndColors.size() / 7;

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
    addVert(A, color);
    addVert(B, color);
    addVert(C, color);
    addVert(A, color);
    addVert(C, color);
    addVert(D, color);

    const size_t index = triangleVertsAndColors.size() / 7;

    triangleIndices.push_back(index + 0);
    triangleIndices.push_back(index + 1);
    triangleIndices.push_back(index + 2);
    triangleIndices.push_back(index + 3);
    triangleIndices.push_back(index + 4);
    triangleIndices.push_back(index + 5);

    /*
    size_t curr = triangleVertsAndColors.size();

    triangleVertsAndColors.resize(curr + ((3 + 4) * 6));

    size_t currIndex = curr / 7;

    triangleIndices.resize(triangleIndices.size() + 6);

    GLfloat * triangleVertsAndColorsPtr = &triangleVertsAndColors[curr];
    GLuint  * triangleIndicesPtr        = &triangleIndices[currIndex];

    //dmess("currIndex " << currIndex);

    addTriangle(A,
                B,
                C,
                color,
                currIndex,
                triangleVertsAndColorsPtr,
                triangleIndicesPtr);
    
    addTriangle(A,
                C,
                D,
                color,
                currIndex,
                triangleVertsAndColorsPtr,
                triangleIndicesPtr);
    */


    //dmess("acurrIndex " << currIndex);
}

void DeferredRenderable::render(const mat4 & MVP, const mat4 & MV) const
{
    dmess("DeferredRenderable::render");

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER,         vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    ColorVertexShader::getInstance()->bind(MVP, MV, false);

    ColorVertexShader::getInstance()->enableVertexAttribArray(3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    ColorVertexShader::getInstance()->enableColorAttribArray(4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GL_FLOAT)));

    glDrawElements(GL_TRIANGLES, numTriIndices, GL_UNSIGNED_INT, NULL);
}