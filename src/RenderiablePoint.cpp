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

    const float size = 0.02;

    const vec3 verts[] = {  pos + vec3(-size, 0, 0),
                            pos + vec3( size, 0, 0),
                            pos + vec3(0, -size, 0),
                            pos + vec3(0, size, 0) };

    float f = *(float *)&verts[0];
    dmess("f " << f);

    //const GLint indices[] = {

    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * tess.numTriangles * 3, tess.triangleIndices, GL_STATIC_DRAW);

    //return new RenderiablePoint(vao, ebo, vbo);
    return NULL;
}

void RenderiablePoint::render(const mat4 & MVP) const
{
    return;

    getDefaultShader()->bind();

    getDefaultShader()->setMVP(MVP);

    getDefaultShader()->setColor(outlineColor);
    
    //glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    //glVertexPointer(3, GL_FLOAT, sizeof(vertex), (void*)(sizeof( float ) * 0));
    
    
    getDefaultShader()->enableVertexAttribArray(3);

    //*
    glDrawArrays(GL_LINE_STRIP, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //getDefaultShader()->unbind();
    //*/

    //getDefaultShader()->enableVertexAttribArray(2);
}

RenderiablePoint::RenderiablePoint( const GLuint      vao,
                                    const GLuint      eao,
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
                                                                    eao(eao),
                                                                    vbo(vbo)
{

}

RenderiablePoint::~RenderiablePoint()
{
    //glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
}

Shader * getDefaultShader() { return defaultShader ;}