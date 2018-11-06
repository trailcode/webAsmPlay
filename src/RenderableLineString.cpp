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

#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/RenderableLineString.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;

RenderableLineString::RenderableLineString( const GLuint  vao,
                                            const GLuint  ebo,
                                            const GLuint  vbo,
                                            const GLuint  numElements,
                                            const bool    isMulti) :    Renderable(isMulti),
                                                                        vao         (vao),
                                                                        ebo         (ebo),
                                                                        vbo         (vbo),
                                                                        numElements (numElements)
{
}

RenderableLineString::~RenderableLineString()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &ebo);
    glDeleteBuffers     (1, &vbo);
}

Renderable * RenderableLineString::create(  const LineString * lineString,
                                            const mat4       & trans)
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

    FloatVec  verts  (coords.size() * 2);
    Uint32Vec indices(coords.size());

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

    return create(  verts,
                    indices,
                    false // isMulti
                );
}

Renderable * RenderableLineString::create(  const ConstGeosGeomVec  & lineStrings,
                                            const mat4              & trans,
                                            const bool                showProgress)
{
    time_point<system_clock> startTime;
    
    if(showProgress) { startTime = system_clock::now() ;}

    size_t numVerts = 0;

    for(const Geometry * ls : lineStrings) { numVerts += dynamic_cast<const LineString *>(ls)->getNumPoints() ;}

    FloatVec  verts(numVerts * 2);
    Uint32Vec indices; // TODO try not to push_back

    GLfloat * vertsPtr = &verts[0];

    size_t index = 0;

    for(size_t i = 0; i < lineStrings.size(); ++i)
    {
        if(showProgress) { doProgress("(3/6) Creating geometry:", i, lineStrings.size(), startTime) ;}

        const vector<Coordinate> & coords = *dynamic_cast<const LineString *>(lineStrings[i])->getCoordinatesRO()->toVector();

        if(trans == mat4(1.0))
        {
            dmess("Fix!");

            abort();
        }
        else
        {
            vec4 v = trans * vec4(coords[0].x, coords[0].y, 0, 1);

            *vertsPtr = v.x; ++vertsPtr;
            *vertsPtr = v.y; ++vertsPtr;

            indices.push_back(index++);

            for(size_t i = 1; i < coords.size() - 1; ++i)
            {
                const vec4 v = trans * vec4(coords[i].x, coords[i].y, 0, 1);

                *vertsPtr = v.x; ++vertsPtr;
                *vertsPtr = v.y; ++vertsPtr;

                indices.push_back(index);
                indices.push_back(index);
                
                ++index;
            }

            v = trans * vec4(coords.rbegin()->x, coords.rbegin()->y, 0, 1);

            *vertsPtr = v.x; ++vertsPtr;
            *vertsPtr = v.y; ++vertsPtr;

            indices.push_back(index++);
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
    GLuint vao = 0;
    GLuint ebo = 0; // Try to remove
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao); // TODO check for errors! Return null if error!
    glGenBuffers     (1, &ebo);
    glGenBuffers     (1, &vbo);
    
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), &verts[0], GL_STATIC_DRAW);

    return new RenderableLineString(vao,
                                    ebo,
                                    vbo,
                                    indices.size(),
                                    isMulti);
}

void RenderableLineString::render(const mat4 & MVP, const mat4 & MV) const
{
    shader->bind(MVP, MV, true);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    shader->enableVertexAttribArray();

    if(!isMulti) { glDrawElements(GL_LINE_STRIP, numElements, GL_UNSIGNED_INT, NULL) ;}
    else         { glDrawElements(GL_LINES,      numElements, GL_UNSIGNED_INT, NULL) ;}
}

