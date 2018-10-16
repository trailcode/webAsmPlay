#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderiablePolygon2D.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

namespace
{
    Shader * outlineShader = NULL;
}

RenderiablePolygon2D::RenderiablePolygon2D( const GLuint            vao,
                                            const GLuint            ebo,
                                            const GLuint            ebo2,
                                            const GLuint            vbo,
                                            const int               numTriangles,
                                            const vector<GLuint> &  counterVertIndices,
                                            const size_t            numContourLines) :  vao                 (vao),
                                                                                        ebo                 (ebo),
                                                                                        ebo2                (ebo2),
                                                                                        vbo                 (vbo),
                                                                                        numTriangles        (numTriangles),
                                                                                        counterVertIndices  (counterVertIndices),
                                                                                        numContourLines     (numContourLines)
{
}

RenderiablePolygon2D::~RenderiablePolygon2D()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
    glDeleteBuffers     (1, &ebo);
    glDeleteBuffers     (1, &ebo2);
}

RenderiablePolygon2D::TesselationResult RenderiablePolygon2D::tessellatePolygon(const Polygon  * poly, const mat4 & trans)
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
    
    const size_t num = coords.size() - 1;

    if(trans == mat4(1.0))
    {
        for(size_t i = 0; i < num; ++i)
        {
            const Coordinate & C = coords[i];

            verts.push_back(C.x);
            verts.push_back(C.y);

            ret.counterVertIndices2.push_back(i);
            ret.counterVertIndices2.push_back((i + 1) % num);
        }
    }
    else
    {
        for(size_t i = 0; i < num; ++i)
        {
            const Coordinate & C = coords[i];

            const vec4 v = trans * vec4(C.x, C.y, 0, 1);

            verts.push_back(v.x);
            verts.push_back(v.y);

            ret.counterVertIndices2.push_back(i);
            ret.counterVertIndices2.push_back((i + 1) % num);
        }
    }

    ret.counterVertIndices.push_back(0);
    ret.counterVertIndices.push_back(verts.size());

    for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
    {
        const vector<Coordinate> & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad gemetry!");

            return ret;
        }

        const size_t num = coords.size() - 1;

        const size_t offset = verts.size() / 2;

        if(trans == mat4(1.0))
        {
            for(size_t i = 0; i < num; ++i)
            {
                const Coordinate & C = coords[i];

                verts.push_back(C.x);
                verts.push_back(C.y);

                ret.counterVertIndices2.push_back(i + offset);
                ret.counterVertIndices2.push_back(((i + 1) % num) + offset);
            }
        }
        else
        {
            for(size_t i = 0; i < num; ++i)
            {
                const Coordinate & C = coords[i];

                const vec4 v = trans * vec4(C.x, C.y, 0, 1);

                verts.push_back(v.x);
                verts.push_back(v.y);

                ret.counterVertIndices2.push_back(i + offset);
                ret.counterVertIndices2.push_back(((i + 1) % num) + offset);
            }
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

void RenderiablePolygon2D::tesselateMultiPolygon(   const MultiPolygon              * multiPoly,
                                                    const mat4                      & trans,
                                                    vector<const TesselationResult> & tesselationResults)
{
    for(size_t i = 0; i < multiPoly->getNumGeometries(); ++i)
    {
        const Polygon * poly = dynamic_cast<const Polygon *>(multiPoly->getGeometryN(i));

        tesselationResults.push_back(tessellatePolygon(poly, trans));
            
        if(!tesselationResults.rbegin()->vertsOut)
        {
            dmess("Warning tesselation failed!");

            tesselationResults.pop_back();
        }
    }
}

Renderiable * RenderiablePolygon2D::createFromTesselations(const vector<const TesselationResult> & tesselations)
{
    if(!tesselations.size()) { return NULL ;}

    size_t numVerts                 = 0;
    size_t numTriangles             = 0;
    size_t numCounterVertIndices    = 0;
    size_t numCounterVertIndices2   = 0;

    for(const TesselationResult & tess : tesselations)
    {
        numVerts                += tess.numVerts;
        numTriangles            += tess.numTriangles;
        numCounterVertIndices   += tess.counterVertIndices.size();
        numCounterVertIndices2  += tess.counterVertIndices2.size();
    }

    vector<GLfloat> verts                (numVerts * 2);
    vector<GLuint>  triangleIndices      (numTriangles * 3);
    vector<GLuint>  counterVertIndices   (numCounterVertIndices);
    vector<GLuint>  counterVertIndices2  (numCounterVertIndices2);

    GLfloat * vertsPtr               = &verts[0];
    GLuint  * triangleIndicesPtr     = &triangleIndices[0];
    GLuint  * counterVertIndicesPtr  = &counterVertIndices[0];
    GLuint  * counterVertIndicesPtr2 = &counterVertIndices2[0];

    size_t offset = 0;

    for(const TesselationResult & tess : tesselations)
    {
        for(size_t i = 0; i < tess.numVerts * 2; ++i, ++vertsPtr) { *vertsPtr = tess.vertsOut[i] ;}

        for(size_t i = 0; i < tess.numTriangles * 3; ++i, ++triangleIndicesPtr) { *triangleIndicesPtr = tess.triangleIndices[i] + offset ;}

        for(size_t i = 0; i < tess.counterVertIndices.size(); ++i, ++counterVertIndicesPtr) { *counterVertIndicesPtr = tess.counterVertIndices[i] + offset ;}

        for(size_t i = 0; i < tess.counterVertIndices2.size(); ++i, ++counterVertIndicesPtr2) { *counterVertIndicesPtr2 = tess.counterVertIndices2[i] + offset ;}

        offset += tess.numVerts;

        free(tess.vertsOut);
        free(tess.triangleIndices);
    }

    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint ebo2 = 0;
    GLuint vbo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, &triangleIndices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &ebo2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * counterVertIndices2.size(), &counterVertIndices2[0], GL_STATIC_DRAW);

    return new RenderiablePolygon2D(vao,
                                    ebo,
                                    ebo2,
                                    vbo,
                                    numTriangles,
                                    counterVertIndices,
                                    counterVertIndices2.size());
}

Renderiable * RenderiablePolygon2D::create(const Polygon * poly, const mat4 & trans)
{
    const TesselationResult tess = tessellatePolygon(poly, trans);

    if(!tess.vertsOut) { return NULL ;}

    GLuint vao = 0;
    GLuint ebo = 0;
    GLuint ebo2 = 0;
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

    glGenBuffers(1, &ebo2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * tess.counterVertIndices2.size(), &tess.counterVertIndices2[0], GL_STATIC_DRAW);
    
    free(tess.vertsOut);
    free(tess.triangleIndices);

    glBindVertexArray(0);

    return new RenderiablePolygon2D(vao,
                                    ebo,
                                    ebo2,
                                    vbo,
                                    tess.numTriangles,
                                    tess.counterVertIndices,
                                    tess.counterVertIndices2.size());
}

Renderiable * RenderiablePolygon2D::create(const MultiPolygon * multiPoly, const mat4 & trans)
{
    vector<const TesselationResult> tesselationResults;

    tesselateMultiPolygon(multiPoly, trans, tesselationResults);

    return createFromTesselations(tesselationResults);
}

Renderiable * RenderiablePolygon2D::create(const vector<const Geometry *> & polygons, const mat4 & trans)
{
    vector<const TesselationResult> tesselationResults;

    for(const Geometry * geom : polygons)
    {
        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if(poly = dynamic_cast<const Polygon *>(geom))
        {
            tesselationResults.push_back(tessellatePolygon(poly, trans));
            
            if(!tesselationResults.rbegin()->vertsOut)
            {
                dmess("Warning tesselation failed!");

                tesselationResults.pop_back();
            }

        }
        else if(multiPoly = dynamic_cast<const MultiPolygon *>(geom))
        {
            tesselateMultiPolygon(multiPoly, trans, tesselationResults);
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
        }
    }

    dmess("tesselationResults " << tesselationResults.size());

    return createFromTesselations(tesselationResults);
}

void RenderiablePolygon2D::render(const mat4 & MVP) const
{
    getDefaultShader()->bind();

    getDefaultShader()->setMVP(MVP);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    getDefaultShader()->enableVertexAttribArray();

    if(getRenderFill())
    {
        getDefaultShader()->setColor(fillColor);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, NULL);
        
        glDisable(GL_BLEND);
    }

    if(getRenderOutline())
    {
        getDefaultShader()->setColor(outlineColor);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
        
        glDrawElements(GL_LINES, numContourLines, GL_UNSIGNED_INT, NULL);
    }

    glBindVertexArray(0); 
}

void RenderiablePolygon2D::ensureShaders()
{
    ensureOutlineShader();
}

void RenderiablePolygon2D::ensureOutlineShader()
{
#ifdef __EMSCRIPTEN__

    return; // WebGL does not support geometry shaders.

#endif

    if(outlineShader) { return ;}

    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 position;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform vec4 vertexColorIn;

        void main()
        {
            gl_Position = MVP * vec4(position.xy, 0, 1);
            vertexColor = vertexColorIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        void main()
        {
            outColor = vec4(1,0,0,1);
        }
    )glsl";

    const GLchar* geometrySource = R"glsl(#version 330 core
        
        layout(points) in;
        layout(line_strip, max_vertices = 2) out;

        void main()
        {
            gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
            EmitVertex();

            gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
            EmitVertex();

            EndPrimitive();
        }

    )glsl";

    outlineShader = Shader::create(vertexSource, fragmentSource, geometrySource);

    dmess("outlineShader " << outlineShader);
}

