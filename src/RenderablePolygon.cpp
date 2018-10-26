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

#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderablePolygon.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

namespace
{
    Shader * outlineShader      = NULL;
    Shader * colorPolygonShader = NULL;
}

// TODO Create better variable names. Try to simplify this class.

RenderablePolygon::RenderablePolygon(   const GLuint           vao,
                                        const GLuint           ebo,
                                        const GLuint           ebo2,
                                        const GLuint           vbo,
                                        const int              numTriangles,
                                        const vector<GLuint> & counterVertIndices,
                                        const size_t           numContourLines,
                                        const bool             isMulti,
                                        const vec4           & fillColor,
                                        const vec4           & outlineColor,
                                        const bool             renderOutline,
                                        const bool             renderFill,
                                        const bool             seperateFillColors) : Renderable(isMulti,
                                                                                                fillColor,
                                                                                                outlineColor,
                                                                                                renderOutline,
                                                                                                renderFill),
                                                                                vao                 (vao),
                                                                                ebo                 (ebo),
                                                                                ebo2                (ebo2),
                                                                                vbo                 (vbo),
                                                                                numTriangles        (numTriangles),
                                                                                counterVertIndices  (counterVertIndices),
                                                                                numContourLines     (numContourLines),
                                                                                seperateFillColors  (seperateFillColors)
{
}

RenderablePolygon::~RenderablePolygon()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
    glDeleteBuffers     (1, &ebo);
}

RenderablePolygon::TesselationResult RenderablePolygon::tessellatePolygon(  const Polygon * poly,
                                                                            const mat4    & trans,
                                                                            const vec4    & fillColor)
{
    TesselationResult ret;

    ret.fillColor = fillColor;

    const LineString * ring = poly->getExteriorRing();

    const vector<Coordinate> & coords = *ring->getCoordinatesRO()->toVector();

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

void RenderablePolygon::tesselateMultiPolygon(  const MultiPolygon              * multiPoly,
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

Renderable * RenderablePolygon::create( const Polygon   * poly,
                                        const mat4      & trans,
                                        const vec4      & fillColor,
                                        const vec4      & outlineColor,
                                        const bool        renderOutline,
                                        const bool        renderFill)
{
    const TesselationResult tess = tessellatePolygon(poly, trans);

    if(!tess.vertsOut) { return NULL ;}

    vector<GLfloat> verts2(tess.numVerts * 2);

    for(size_t i = 0; i < tess.numVerts * 2; ++i) { verts2[i] = tess.vertsOut[i] ;}

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;

    glGenVertexArrays   (1, &vao);
    glGenBuffers        (1, &vbo);
    glGenBuffers        (1, &ebo);
    glGenBuffers        (1, &ebo2);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tess.numVerts * 2, &verts2[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * tess.numTriangles * 3, tess.triangleIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * tess.counterVertIndices2.size(), &tess.counterVertIndices2[0], GL_STATIC_DRAW);
    
    free(tess.vertsOut);
    free(tess.triangleIndices);

    glBindVertexArray(0);

    return new RenderablePolygon(   vao,
                                    ebo,
                                    ebo2,
                                    vbo,
                                    tess.numTriangles,
                                    tess.counterVertIndices,
                                    tess.counterVertIndices2.size(),
                                    false, // Is is a multi-polygon
                                    fillColor,
                                    outlineColor,
                                    renderOutline,
                                    renderFill,
                                    false);
}

Renderable * RenderablePolygon::create( const MultiPolygon  * multiPoly,
                                        const mat4          & trans,
                                        const vec4          & fillColor,
                                        const vec4          & outlineColor,
                                        const bool            renderOutline,
                                        const bool            renderFill)
{
    vector<const TesselationResult> tesselationResults;

    tesselateMultiPolygon(multiPoly, trans, tesselationResults);

    return createFromTesselations(  tesselationResults,
                                    fillColor,
                                    outlineColor,
                                    renderOutline, 
                                    renderFill,
                                    false);
}

Renderable * RenderablePolygon::create( const vector<const Geometry *>  & polygons,
                                        const mat4                      & trans,
                                        const vec4                      & fillColor,
                                        const vec4                      & outlineColor,
                                        const bool                        renderOutline,
                                        const bool                        renderFill)
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
            dmess("Have a multiPoly!");

            tesselateMultiPolygon(multiPoly, trans, tesselationResults);
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
        }
    }

    dmess("tesselationResults " << tesselationResults.size());

    return createFromTesselations(  tesselationResults,
                                    fillColor,
                                    outlineColor,
                                    renderOutline,
                                    renderFill,
                                    false);
}

Renderable * RenderablePolygon::create( const vector<tuple<const Geometry *, const vec4, const vec4> >  & polygons,
                                        const mat4                      & trans,
                                        const vec4                      & fillColor,
                                        const vec4                      & outlineColor,
                                        const bool                        renderOutline,
                                        const bool                        renderFill)
{
    vector<const TesselationResult> tesselationResults;

    for(const tuple<const Geometry *, const vec4, const vec4> & polyAndColors : polygons)
    {
        const Geometry  * geom          = get<0>(polyAndColors);
        const vec4      & fillColor     = get<1>(polyAndColors);
        const vec4      & outlineColor  = get<2>(polyAndColors);

        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if(poly = dynamic_cast<const Polygon *>(geom))
        {
            tesselationResults.push_back(tessellatePolygon(poly, trans, fillColor));
            
            if(!tesselationResults.rbegin()->vertsOut)
            {
                dmess("Warning tesselation failed!");

                tesselationResults.pop_back();
            }
        }
        else if(multiPoly = dynamic_cast<const MultiPolygon *>(geom))
        {
            dmess("Have a multiPoly!");

            //tesselateMultiPolygon(multiPoly, trans, tesselationResults);
            abort();
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
            abort();
        }
    }

    return createFromTesselations(  tesselationResults,
                                    fillColor,
                                    outlineColor,
                                    renderOutline,
                                    renderFill,
                                    true);
}

Renderable * RenderablePolygon::createFromTesselations( const vector<const TesselationResult>   & tesselations,
                                                        const vec4                              & fillColor,
                                                        const vec4                              & outlineColor,
                                                        const bool                                renderOutline,
                                                        const bool                                renderFill,
                                                        const bool                                seperateFillColors)
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

    vector<GLfloat> verts;
    
    if(seperateFillColors)  { verts.resize(numVerts * (2 + 4)) ;}
    else                    { verts.resize(numVerts * 2 + 4) ;}

    vector<GLuint>  triangleIndices      (numTriangles * 3);
    vector<GLuint>  counterVertIndices   (numCounterVertIndices);
    vector<GLuint>  counterVertIndices2  (numCounterVertIndices2);

    GLfloat * vertsPtr               = &verts[0];
    GLuint  * triangleIndicesPtr     = &triangleIndices[0];
    GLuint  * counterVertIndicesPtr  = &counterVertIndices[0];
    GLuint  * counterVertIndicesPtr2 = &counterVertIndices2[0];

    size_t offset = 0;

    for(size_t i = 0; i < tesselations.size(); ++i)
    {
        const TesselationResult & tess = tesselations[i];

        if(seperateFillColors) // TODO template or make two loops to avoid so many if checks
        {
            for(size_t j = 0; j < tess.numVerts; ++j)
            {
                *vertsPtr = tess.vertsOut[j * 2 + 0]; ++vertsPtr;
                *vertsPtr = tess.vertsOut[j * 2 + 1]; ++vertsPtr;

                *vertsPtr = tesselations[i].fillColor.x; ++vertsPtr;
                *vertsPtr = tesselations[i].fillColor.y; ++vertsPtr;
                *vertsPtr = tesselations[i].fillColor.z; ++vertsPtr;
                *vertsPtr = tesselations[i].fillColor.w; ++vertsPtr;
            }
        }
        else
        {
            for(size_t j = 0; j < tess.numVerts * 2; ++j, ++vertsPtr) { *vertsPtr = tess.vertsOut[j] ;}
        }

        for(size_t j = 0; j < tess.numTriangles * 3; ++j, ++triangleIndicesPtr) { *triangleIndicesPtr = tess.triangleIndices[j] + offset ;}

        for(size_t j = 0; j < tess.counterVertIndices.size(); ++j, ++counterVertIndicesPtr) { *counterVertIndicesPtr = tess.counterVertIndices[j] + offset ;}

        for(size_t j = 0; j < tess.counterVertIndices2.size(); ++j, ++counterVertIndicesPtr2) { *counterVertIndicesPtr2 = tess.counterVertIndices2[j] + offset ;}

        offset += tess.numVerts;

        free(tess.vertsOut);
        free(tess.triangleIndices);
    }

    GLuint vao  = 0;
    GLuint ebo  = 0;
    GLuint ebo2 = 0;
    GLuint vbo  = 0;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &ebo2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if(!seperateFillColors) { glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts[0], GL_STATIC_DRAW) ;}
    else                    { glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * (2 + 4), &verts[0], GL_STATIC_DRAW) ;}
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, &triangleIndices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * counterVertIndices2.size(), &counterVertIndices2[0], GL_STATIC_DRAW);

    return new RenderablePolygon(   vao,
                                    ebo,
                                    ebo2,
                                    vbo,
                                    numTriangles,
                                    counterVertIndices,
                                    counterVertIndices2.size(),
                                    tesselations.size() > 1, // Is is a multi-polygon
                                    fillColor,
                                    outlineColor,
                                    renderOutline,
                                    renderFill,
                                    seperateFillColors);
}

void RenderablePolygon::render(const mat4 & MVP) const
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    //glClear(GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_DEPTH_TEST);

    if(seperateFillColors)
    {
        colorPolygonShader->bind();

        colorPolygonShader->setMVP(MVP);

        colorPolygonShader->enableVertexAttribArray(2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

        colorPolygonShader->enableColorAttribArray(4, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
    }
    else
    {
        getDefaultShader()->bind();

        getDefaultShader()->setMVP(MVP);

        if(getRenderFill()) { getDefaultShader()->setColor(fillColor) ;}

        getDefaultShader()->enableVertexAttribArray();
    }

    if(getRenderFill())
    {
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, NULL);
        
        glDisable(GL_BLEND);
    }

    glDisable(GL_DEPTH_TEST);

    if(getRenderOutline())
    {
        if(seperateFillColors)
        {
            getDefaultShader()->bind();

            getDefaultShader()->setMVP(MVP);

            getDefaultShader()->enableVertexAttribArray(2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        } 
        
        getDefaultShader()->setColor(outlineColor);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
        
        glDrawElements(GL_LINES, numContourLines, GL_UNSIGNED_INT, NULL);
    }
}

void RenderablePolygon::ensureShaders()
{
    ensureOutlineShader();
    ensureColorPolygonShader();
}

void RenderablePolygon::ensureOutlineShader()
{
#ifdef __EMSCRIPTEN__

    return; // WebGL does not support geometry shaders.

#endif

    if(outlineShader) { return ;}

    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 vertIn;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform vec4 colorIn;

        void main()
        {
            gl_Position = MVP * vec4(vertIn.xy, 0, 1);
            vertexColor = colorIn;
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

void RenderablePolygon::ensureColorPolygonShader()
{
    if(colorPolygonShader) { return ;}

    dmess("RenderablePolygon::ensureColorPolygonShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 vertIn;
        in vec4 vertColorIn;
        out vec4 vertexColor;

        uniform mat4 MVP;

        void main()
        {
            gl_Position = MVP * vec4(vertIn.xy, 0, 1);
            vertexColor = vertColorIn;
            //vertexColor = vec4(0,1,1,1);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        void main()
        {
            outColor = vertexColor;
        }
    )glsl";

    colorPolygonShader = Shader::create(vertexSource, fragmentSource);
}