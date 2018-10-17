#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderiablePoint.h>

using namespace std;
using namespace glm;

namespace
{
    Shader * defaultShader = NULL;
}

Renderiable * RenderiablePoint::create(const vec3 & pos)
{
    dmess("RenderiablePoint::create");

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

    return new RenderiablePoint(vao, ebo, vbo,
    false,
                        getDefaultFillColor(),
                        getDefaultOutlineColor(),
                        true,
                        true);
}

void RenderiablePoint::render(const mat4 & MVP) const
{
    getDefaultShader()->bind();

    getDefaultShader()->setMVP(MVP);

    getDefaultShader()->setColor(outlineColor);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    getDefaultShader()->enableVertexAttribArray(3);

    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, NULL);
}

RenderiablePoint::RenderiablePoint( const GLuint      vao,
                                    const GLuint      ebo,
                                    const GLuint      vbo,
                                    const bool        isMulti,
                                    const vec4      & fillColor,
                                    const vec4      & outlineColor,
                                    const bool        renderOutline,
                                    const bool        renderFill) : Renderiable(isMulti,
                                                                                fillColor,
                                                                                outlineColor,
                                                                                renderOutline,
                                                                                renderFill),
                                                                    vao(vao),
                                                                    ebo(ebo),
                                                                    vbo(vbo)
{

}

RenderiablePoint::~RenderiablePoint()
{
    //glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
}

Shader * getDefaultShader() { return defaultShader ;}