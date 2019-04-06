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

#include <geos/geom/Point.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/renderables/RenderablePoint.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

Renderable * RenderablePoint::create(const Point * point,
                                     const dmat4 & trans)
{
    return create(dvec3(point->getX(), point->getY(), point->getZ()), trans);
}

Renderable * RenderablePoint::create(const dvec3 & _pos,
                                     const dmat4 & trans)
{
    const float size = 0.05;

    const vec3 pos = trans * dvec4(_pos, 1);

    const vec3 verts[] = {  pos + vec3(-size,  0,    0),
                            pos + vec3( size,  0,    0),
                            pos + vec3(0,     -size, 0),
                            pos + vec3(0,      size, 0) };

    const GLuint indices[] = {0,1,2,3};

    //GLuint vao = 0;
    GLuint ebo = 0;
    GLuint vbo = 0;

    //GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glGenBuffers     (1, &ebo));
    GL_CHECK(glGenBuffers     (1, &vbo));

    //GL_CHECK(glBindVertexArray(vao));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    return new RenderablePoint(ebo, vbo, false);
}

Renderable * RenderablePoint::create(const ConstGeosGeomVec & points,
                                     const dmat4            & trans,
                                     const bool               showProgress)
{
    //abort();
    for(const auto i : points)
    {
        const dvec2 P(dynamic_cast<const Point *>(i)->getX(), dynamic_cast<const Point *>(i)->getY());

        dmess("P " << P.x << " " << P.y);
    }

    return NULL;
}

void RenderablePoint::render(Canvas * canvas, const size_t renderStage)
{
    if(!shader->shouldRender(true, renderStage)) { return ;}

    shader->bind(canvas, false, renderStage);

    GL_CHECK(glBindVertexArray(                    vao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER,         vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

    GL_CHECK(glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, NULL));
}

RenderablePoint::RenderablePoint(   const GLuint      ebo,
                                    const GLuint      vbo,
                                    const bool        isMulti) : Renderable(isMulti,
                                                                            false,
                                                                            false),
                                                                 ebo       (ebo),
                                                                 vbo       (vbo)
{

}

RenderablePoint::~RenderablePoint()
{
    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers     (1, &vbo));
}

void RenderablePoint::ensureVAO()
{
	GL_CHECK(glGenVertexArrays(1, &vao));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));


}