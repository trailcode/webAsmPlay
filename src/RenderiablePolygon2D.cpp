#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderiablePolygon2D.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

RenderiablePolygon2D::RenderiablePolygon2D( const GLuint            vao,
                                            const GLuint            ebo,
                                            const GLuint            vbo,
                                            const int               numTriangles,
                                            const vector<GLuint> &  counterVertIndices) :   vao                 (vao),
                                                                                            ebo                 (ebo),
                                                                                            vbo                 (vbo),
                                                                                            numTriangles        (numTriangles),
                                                                                            counterVertIndices  (counterVertIndices)
{
}

RenderiablePolygon2D::~RenderiablePolygon2D()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
    glDeleteBuffers     (1, &ebo);
}

namespace
{
    struct TesselationResult
    {
        double  * vertsOut          = NULL;
        int     * triangleIndices   = NULL;
        int       numVerts          = 0;
        int       numTriangles      = 0;

        vector<GLuint> counterVertIndices;
    };

    TesselationResult tessellatePolygon(const Polygon  * poly, const mat4 & trans)
    {
        TesselationResult ret;

        const LineString * ring = poly->getExteriorRing();

        const vector<Coordinate> & coords = *ring->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad gemetry!");

            return ret;
        }

        vector<double> verts;
        
        if(trans == mat4(1.0))
        {
            for(size_t i = 0; i < coords.size() - 1; ++i)
            {
                const Coordinate & C = coords[i];

                verts.push_back(C.x);
                verts.push_back(C.y);
            }
        }
        else
        {
            for(size_t i = 0; i < coords.size() - 1; ++i)
            {
                const Coordinate & C = coords[i];

                const vec4 v = trans * vec4(C.x, C.y, 0, 1);

                verts.push_back(v.x);
                verts.push_back(v.y);
            }
        }

        ret.counterVertIndices.push_back(0);
        ret.counterVertIndices.push_back(verts.size());
        
        //dmess("verts.size() " << verts.size());

        for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
        {
            const vector<Coordinate> & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

            if(coords.size() < 4)
            {
                dmess("Bad gemetry!");

                return ret;
            }

            if(trans == mat4(1.0))
            {
                for(size_t i = 0; i < coords.size() - 1; ++i)
                {
                    const Coordinate & C = coords[i];

                    const vec4 v = trans * vec4(C.x, C.y, 0, 1);

                    verts.push_back(v.x);
                    verts.push_back(v.y);
                }
            }
            else
            {

            }

            ret.counterVertIndices.push_back(verts.size());
        }

        vector<const double *> counterVertPtrs;

        for(size_t i = 0; i < ret.counterVertIndices.size(); ++i) { counterVertPtrs.push_back(&verts[0] + ret.counterVertIndices[i]) ;}

        tessellate( &ret.vertsOut,
                    &ret.numVerts,
                    &ret.triangleIndices,
                    &ret.numTriangles,
                    &counterVertPtrs[0],
                    &counterVertPtrs[0] + counterVertPtrs.size());

        for(size_t i = 0; i < ret.counterVertIndices.size(); ++i) { ret.counterVertIndices[i] /= 2 ;}

        return ret;
    }
}

Renderiable * RenderiablePolygon2D::create(const Polygon * poly, const mat4 & trans)
{
    ensureShader();

    const TesselationResult tess = tessellatePolygon(poly, trans);

    if(!tess.vertsOut) { return NULL ;}

    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    vector<GLfloat> verts2(tess.numVerts * 2);

    for(size_t i = 0; i < tess.numVerts * 2; ++i) { verts2[i] = tess.vertsOut[i] ;}

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tess.numVerts * 2, &verts2[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * tess.numTriangles * 3, tess.triangleIndices, GL_STATIC_DRAW);
    
    free(tess.vertsOut);
    free(tess.triangleIndices);

    return new RenderiablePolygon2D(vao,
                                    ebo,
                                    vbo,
                                    tess.numTriangles,
                                    tess.counterVertIndices);
}

Renderiable * RenderiablePolygon2D::create(const MultiPolygon * multyPoly, const mat4 & trans)
{
    ensureShader();

    vector<const TesselationResult> tesselationResults;

    for(size_t i = 0; i < multyPoly->getNumGeometries(); ++i)
    {
        const Polygon * poly = dynamic_cast<const Polygon *>(multyPoly->getGeometryN(i));

        tesselationResults.push_back(tessellatePolygon(poly, trans));
            
        if(!tesselationResults.rbegin()->vertsOut) { return NULL ;}
    }

    size_t numVerts                 = 0;
    size_t numTriangles             = 0;
    size_t numCounterVertIndices    = 0;

    for(const TesselationResult & tess : tesselationResults)
    {
        numVerts                += tess.numVerts;
        numTriangles            += tess.numTriangles;
        numCounterVertIndices   += tess.counterVertIndices.size();
    }

    vector<GLfloat> verts               (numVerts * 2);
    vector<GLuint>  triangleIndices     (numTriangles * 3);
    vector<GLuint>  counterVertIndices  (numCounterVertIndices);

    GLfloat * vertsPtr              = &verts[0];
    GLuint  * triangleIndicesPtr    = &triangleIndices[0];
    GLuint  * counterVertIndicesPtr = &counterVertIndices[0];

    size_t offset = 0;

    for(const TesselationResult & tess : tesselationResults)
    {
        for(size_t i = 0; i < tess.numVerts * 2; ++i, ++vertsPtr) { *vertsPtr = tess.vertsOut[i] ;}

        for(size_t i = 0; i < tess.numTriangles * 3; ++i, ++triangleIndicesPtr) { *triangleIndicesPtr = tess.triangleIndices[i] + offset ;}

        for(size_t i = 0; i < tess.counterVertIndices.size(); ++i, ++counterVertIndicesPtr) { *counterVertIndicesPtr = tess.counterVertIndices[i] + offset ;}

        offset += tess.numVerts;

        free(tess.vertsOut);
        free(tess.triangleIndices);
    }

    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, &triangleIndices[0], GL_STATIC_DRAW);
    
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

