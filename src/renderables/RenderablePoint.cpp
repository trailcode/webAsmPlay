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
    const float size = 0.05f;

    const vec3 pos = trans * dvec4(_pos, 1);

    const vec3 verts[] = {  pos + vec3(-size,  0,    0),
                            pos + vec3( size,  0,    0),
                            pos + vec3(0,     -size, 0),
                            pos + vec3(0,      size, 0) };

    const GLuint indices[] = {0,1,2,3};

    GLuint ebo = 0;
    GLuint vbo = 0;

    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Flush is required if executing in a thread different from the main thread.
	glFlush();

    return new RenderablePoint(ebo, vbo, false);
}

Renderable * RenderablePoint::create(const ConstGeosGeomVec & points,
                                     const dmat4            & trans,
                                     const bool               showProgress)
{
    for(const auto i : points)
    {
        const dvec2 P(dynamic_cast<const Point *>(i)->getX(), dynamic_cast<const Point *>(i)->getY());

        dmess("P " << P.x << " " << P.y);
    }

    return nullptr;
}

void RenderablePoint::render(Canvas * canvas, const size_t renderStage)
{
    if(!m_shader->m_shouldRender(true, renderStage)) { return ;}

    m_shader->bind(canvas, false, renderStage);

    glBindVertexArray(                    m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

RenderablePoint::RenderablePoint(   const GLuint      ebo,
                                    const GLuint      vbo,
                                    const bool        isMulti) : Renderable(isMulti,
                                                                            false,
                                                                            false),
                                                                 m_ebo     (ebo),
                                                                 m_vbo     (vbo)
{

}

RenderablePoint::~RenderablePoint()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers     (1, &m_vbo);
}

void RenderablePoint::ensureVAO()
{
	if(m_vao) { return ;}

	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
}