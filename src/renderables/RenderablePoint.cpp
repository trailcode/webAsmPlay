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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/renderables/RenderablePoint.h>

using namespace std;
using namespace glm;

Renderable * RenderablePoint::create(const vec3 & pos,
                                     const mat4 & trans)
{
    const float size = 0.05;

    const vec3 verts[] = {  pos + vec3(-size, 0, 0),
                            pos + vec3( size, 0, 0),
                            pos + vec3(0, -size, 0),
                            pos + vec3(0, size, 0) };

    const GLuint indices[] = {0,1,2,3};

    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    return new RenderablePoint(vao, ebo, vbo, false);
}

Renderable * RenderablePoint::create(const ConstGeosGeomVec & points,
                                     const mat4             & trans,
                                     const bool               showProgress)
{
    abort();
    return NULL;
}

void RenderablePoint::render(const mat4 & model,
                             const mat4 & view,
                             const mat4 & projection) const
{
    shader->bind(model, view, projection, false);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    shader->enableVertexAttribArray(3);

    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, NULL);
}

RenderablePoint::RenderablePoint(   const GLuint      vao,
                                    const GLuint      ebo,
                                    const GLuint      vbo,
                                    const bool        isMulti) : Renderable(isMulti),
                                                                 vao       (vao),
                                                                 ebo       (ebo),
                                                                 vbo       (vbo)
{

}

RenderablePoint::~RenderablePoint()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
}
