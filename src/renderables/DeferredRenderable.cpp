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
    vector<GLfloat> vertsAndColors;
    vector<GLuint>  triangleIndices;
    vector<GLuint>  lineIndices;

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

        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
        *triangleIndicesPtr = startIndex++; ++triangleIndicesPtr;
    }
}

DeferredRenderable * DeferredRenderable::createFromQueued(const dmat4 & trans)
{
	if (!vertsAndColors.size()) { return NULL; }

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
    for(size_t i = 0; i < vertsAndColors.size(); i += 7)
    {
        const vec3 pos(trans * dvec4(*(vec3 *)&vertsAndColors[i], 1.0));

        memcpy(&vertsAndColors[i], &pos, sizeof(vec3));
    }

    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glGenBuffers(1, &ebo));
    GL_CHECK(glGenBuffers(1, &ebo2));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertsAndColors.size() * sizeof(GLfloat), &vertsAndColors[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * lineIndices.size(), &lineIndices[0], GL_STATIC_DRAW));

    // TODO use VertexArrayObject

    const size_t sizeVertex = 3;
    const size_t sizeColor = 4;

    const size_t totalSize = (sizeVertex + sizeColor) * sizeof(GLfloat);

    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glVertexAttribPointer(0, sizeVertex, GL_FLOAT, GL_FALSE, totalSize, 0));

    GL_CHECK(glEnableVertexAttribArray(1));

    GL_CHECK(glVertexAttribPointer(1, sizeColor, GL_FLOAT, GL_FALSE, totalSize, (void *)((sizeVertex) * sizeof(GLfloat))));

    DeferredRenderable * ret = new DeferredRenderable(vao, ebo, ebo2, vbo, triangleIndices.size(), lineIndices.size());

    vertsAndColors .clear();
    triangleIndices.clear();
    lineIndices    .clear();

    return ret;
}

DeferredRenderable::DeferredRenderable( const GLuint & vao,
                                        const GLuint & ebo,
                                        const GLuint & ebo2,
                                        const GLuint & vbo,
                                        const GLuint & numTriIndices,
                                        const GLuint & numLineIndices) : Renderable    (false,
                                                                                        GUI::renderSettingsFillPolygons,
                                                                                        GUI::renderSettingsRenderPolygonOutlines),
                                                                         vao           (vao),
                                                                         ebo           (ebo),
                                                                         ebo2          (ebo2),
                                                                         vbo           (vbo),
                                                                         numTriIndices (numTriIndices),
                                                                         numLineIndices(numLineIndices)
{
    setShader(ColorVertexShader::getDefaultInstance());
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
        vertsAndColors.push_back(v.x * 0.1);
        vertsAndColors.push_back(v.y * 0.1);
        vertsAndColors.push_back(v.z * 0.1);
        vertsAndColors.push_back(color.x);
        vertsAndColors.push_back(color.y);
        vertsAndColors.push_back(color.z);
        vertsAndColors.push_back(color.w);
    }
}

void DeferredRenderable::addLine(const vec3 & A,
                                 const vec3 & B,
                                 const vec4 & color)
{
    const size_t index = vertsAndColors.size() / 7;

    addVert(A, color);
    addVert(B, color);
    
    lineIndices.push_back(index + 0);
    lineIndices.push_back(index + 1);
}

void DeferredRenderable::addTriangle(const vec3 & A,
                                     const vec3 & B,
                                     const vec3 & C,
                                     const vec4 & color)
{
    const size_t index = vertsAndColors.size() / 7;

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
    const size_t index = vertsAndColors.size() / 7;

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
}

void DeferredRenderable::render(Canvas * canvas, const size_t renderStage) const
{
    if(!shader->shouldRender(false, renderStage)) { return ;}

    GL_CHECK(glBindVertexArray(vao));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER,         vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

    glDisable(GL_DEPTH_TEST);

    canvas->pushModel(rotate(canvas->getModelRef(), radians(-90.0), dvec3(1, 0, 0)));

    shader->bind(canvas, false, renderStage);

    GL_CHECK(glDrawElements(GL_TRIANGLES, numTriIndices, GL_UNSIGNED_INT, NULL));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2));

    GL_CHECK(glDrawElements(GL_LINES, numLineIndices, GL_UNSIGNED_INT, NULL));

    canvas->popMVP();
}