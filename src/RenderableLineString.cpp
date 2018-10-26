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

#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderableLineString.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

RenderableLineString::RenderableLineString( const GLuint   vao,
                                            const GLuint   ebo,
                                            const GLuint   vbo,
                                            const GLuint   numVerts,
                                            const bool     isMulti,
                                            const vec4   & fillColor,
                                            const vec4   & outlineColor,
                                            const bool     renderOutline,
                                            const bool     renderFill) :    Renderable(isMulti,
                                                                                        fillColor,
                                                                                        outlineColor,
                                                                                        renderOutline,
                                                                                        renderFill),
                                                                            vao     (vao),
                                                                            ebo     (ebo),
                                                                            vbo     (vbo),
                                                                            numVerts(numVerts)
{
}

RenderableLineString::~RenderableLineString()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &ebo);
    glDeleteBuffers     (1, &vbo);
}

Renderable * RenderableLineString::create(  const LineString    * lineString,
                                            const mat4          & trans,
                                            const vec4          & fillColor,
                                            const vec4          & outlineColor,
                                            const bool            renderOutline,
                                            const bool            renderFill)
{
    if(!lineString)
    {
        dmess("Error lineString is NULL!");

        return NULL;
    }

    const vector<Coordinate> & coords = *lineString->getCoordinatesRO()->toVector();

    if(coords.size() < 2)
    {
        dmess("Bad gemetry!");

        return NULL;
    }

    vector<GLfloat> verts(coords.size() * 2);
    vector<GLuint> indices(coords.size());

    GLfloat * vertsPtr = &verts[0];

    if(trans == mat4(1.0))
    {
        for(size_t i = 0; i < coords.size(); ++i)
        {
            *vertsPtr = coords[i].x; ++vertsPtr;
            *vertsPtr = coords[i].y; ++vertsPtr;

            indices[i] = i;
        }
    }
    else
    {
        for(size_t i = 0; i < coords.size(); ++i)
        {
            const vec4 v = trans * vec4(coords[i].x, coords[i].y, 0, 1);

            *vertsPtr = v.x; ++vertsPtr;
            *vertsPtr = v.y; ++vertsPtr;

            indices[i] = i;
        }
    }

    GLuint vao = 0;
    GLuint ebo = 0; // Try to remove
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * coords.size() * 2, &verts[0], GL_STATIC_DRAW);

    return new RenderableLineString(vao,
                                    ebo,
                                    vbo,
                                    indices.size(),
                                    false,
                                    fillColor,
                                    outlineColor,
                                    renderOutline,
                                    renderFill);
}

void RenderableLineString::render(const mat4 & MVP) const
{
    getDefaultShader()->bind();

    getDefaultShader()->setMVP(MVP);

    getDefaultShader()->setColor(outlineColor);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    getDefaultShader()->enableVertexAttribArray();

    glDrawElements(GL_LINE_STRIP, numVerts, GL_UNSIGNED_INT, NULL);
}