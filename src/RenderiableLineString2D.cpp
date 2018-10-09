#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderiableLineString2D.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

RenderiableLineString2D::RenderiableLineString2D(   const GLuint vao,
                                                    const GLuint vbo,
                                                    const GLuint numVerts) :    vao     (vao),
                                                                                vbo     (vbo),
                                                                                numVerts(numVerts)
{
}

RenderiableLineString2D::~RenderiableLineString2D()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
}

Renderiable * RenderiableLineString2D::create(const LineString * lineString, const mat4 & trans)
{
    ensureShader();

    const vector<Coordinate> & coords = *lineString->getCoordinates()->toVector();

    if(coords.size() < 2)
    {
        dmess("Bad gemetry!");

        return NULL;
    }

    GLuint vao = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    vector<GLfloat> verts(coords.size() * 2);

    GLfloat * vertsPtr = &verts[0];

    if(trans == mat4(1.0))
    {
        for(size_t i = 0; i < coords.size(); ++i)
        {
            *vertsPtr = coords[i].x; ++vertsPtr;
            *vertsPtr = coords[i].y; ++vertsPtr;
        }
    }
    else
    {
        for(size_t i = 0; i < coords.size(); ++i)
        {
            const vec4 v = trans * vec4(coords[i].x, coords[i].y, 0, 1);

            *vertsPtr = v.x; ++vertsPtr;
            *vertsPtr = v.y; ++vertsPtr;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * coords.size() * 2, &verts[0], GL_STATIC_DRAW);

    return new RenderiableLineString2D( vao,
                                        vbo,
                                        coords.size());
}

void RenderiableLineString2D::render(const mat4 & MVP) const
{
    defaultShader->bind();

    defaultShader->setMVP(MVP);

    defaultShader->setColor(outlineColor);
    
    glBindVertexArray(vao);
    
    defaultShader->enableVertexAttribArray();

    glDrawArrays(GL_LINE_STRIP, 0, numVerts);
}