#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderiablePolygon2D.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

RenderiablePolygon2D::RenderiablePolygon2D(const GLuint  vao,
                    const GLuint  ebo,
                    const GLuint  vbo,
                    const int     numTriangles,
                    const vector<size_t> & counterVertIndices) : vao(vao),
                                    ebo(ebo),
                                    vbo(vbo),
                                    numTriangles(numTriangles)
{
}

RenderiablePolygon2D::~RenderiablePolygon2D()
{
    //glDeleteVertexArrays(1, &vao);
    //glDeleteBuffers     (1, &vbo);
    //glDeleteBuffers     (1, &ebo);
}

Renderiable * RenderiablePolygon2D::create(const Polygon * poly)
{
    ensureShader();

    const Envelope * bounds = poly->getEnvelopeInternal();

    Coordinate center;

    if(!bounds->centre(center))
    {
        dmess("Bad geometry!");

        return NULL;
    }

    const LineString * ring = poly->getExteriorRing();

    const vector<Coordinate> & coords = *ring->getCoordinates()->toVector();

    if(coords.size() < 4)
    {
        dmess("Bad gemetry!");

        return NULL;
    }

    vector<double> verts;

    for(size_t i = 0; i < coords.size() - 1; ++i)
    {
        const Coordinate & C = coords[i];

        verts.push_back(C.x);
        verts.push_back(C.y);
    }

    vector<size_t> counterVertIndices;
    
    counterVertIndices.push_back(0);
    counterVertIndices.push_back(verts.size());
    
    //dmess("verts.size() " << verts.size());

    for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
    {
        const vector<Coordinate> & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad gemetry!");

            return NULL;
        }

        for(size_t i = 0; i < coords.size() - 1; ++i)
        {
            const Coordinate & C = coords[i];

            verts.push_back(C.x);
            verts.push_back(C.y);
        }

        counterVertIndices.push_back(verts.size());
    }

    vector<const double *> counterVertPtrs;

    for(size_t i = 0; i < counterVertIndices.size(); ++i) { counterVertPtrs.push_back(&verts[0] + counterVertIndices[i]) ;}

    double  * vertsOut;
    int     * triangleIndices;
    int       numVerts;
    int       numTriangles;

    tessellate( &vertsOut,
                &numVerts,
                &triangleIndices,
                &numTriangles,
                &counterVertPtrs[0],
                &counterVertPtrs[0] + counterVertPtrs.size());

    GLuint vao          = 0;
    GLuint ebo          = 0;
    GLuint eboOutline   = 0;
    GLuint vbo          = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    vector<GLfloat> verts2(numVerts * 2);

    for(size_t i = 0; i < numVerts * 2; ++i) { verts2[i] = vertsOut[i] ;}

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts2[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, triangleIndices, GL_STATIC_DRAW);
    
    free(vertsOut);
    if(triangleIndices) { free(triangleIndices) ;}

    for(size_t i = 0; i < counterVertIndices.size(); ++i) { counterVertIndices[i] /= 2 ;}

    return new RenderiablePolygon2D(vao,
                                    ebo,
                                    vbo,
                                    numTriangles,
                                    counterVertIndices);
}

void RenderiablePolygon2D::render(const mat4 & MVP) const
{
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(MVP_Attrib, 1, false, glm::value_ptr(MVP));

    glUniform4f(colorAttrib, fillColor.x, fillColor.y, fillColor.z, fillColor.w);

    glBindVertexArray(vao);
    
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, 0);
    
    glDisable(GL_BLEND);

    glUniform4f(colorAttrib, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w);
    
    for(size_t i = 0; i < counterVertIndices.size() - 1; ++i)
    {
        const size_t a = counterVertIndices[i];
        const size_t b = counterVertIndices[i + 1];

        glDrawArrays(GL_LINE_LOOP, a, (b - a));
    }
}

