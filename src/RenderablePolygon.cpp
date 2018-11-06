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

#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <GLU/tessellate.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/RenderablePolygon.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;

// TODO Create better variable names. Try to simplify this class.

RenderablePolygon::RenderablePolygon(   const GLuint      vao,
                                        const GLuint      ebo,
                                        const GLuint      ebo2,
                                        const GLuint      vbo,
                                        const int         numTriangles,
                                        const Uint32Vec & counterVertIndices,
                                        const size_t      numContourLines,
                                        const bool        isMulti) : Renderable          (isMulti),
                                                                     vao                 (vao),
                                                                     ebo                 (ebo),
                                                                     ebo2                (ebo2),
                                                                     vbo                 (vbo),
                                                                     numTriangles        (numTriangles),
                                                                     counterVertIndices  (counterVertIndices),
                                                                     numContourLines     (numContourLines)
{
}

RenderablePolygon::~RenderablePolygon()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers     (1, &vbo);
    glDeleteBuffers     (1, &ebo);
    glDeleteBuffers     (1, &ebo2);
}

RenderablePolygon::TessellationResult RenderablePolygon::tessellatePolygon( const Polygon * poly,
                                                                            const dmat4   & trans,
                                                                            const size_t    symbologyID)
{
    TessellationResult ret;

    ret.symbologyID = symbologyID;

    const LineString * ring = poly->getExteriorRing();

    const vector<Coordinate> & coords = *ring->getCoordinatesRO()->toVector();

    if(coords.size() < 4)
    {
        dmess("Bad geometry!");

        return ret;
    }

    vector<double> verts;
    
    const size_t num = coords.size() - 1;

    if(trans == dmat4(1.0))
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

            const dvec4 v = trans * dvec4(C.x, C.y, 0, 1);

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
            dmess("Bad geometry!");

            return ret;
        }

        const size_t num = coords.size() - 1;

        const size_t offset = verts.size() / 2;

        if(trans == dmat4(1.0))
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

void RenderablePolygon::tessellateMultiPolygon( const MultiPolygon  * multiPoly,
                                                const dmat4         & trans,
                                                Tessellations       & tessellationResults,
                                                const size_t          symbologyID)
{
    for(size_t i = 0; i < multiPoly->getNumGeometries(); ++i)
    {
        const Polygon * poly = dynamic_cast<const Polygon *>(multiPoly->getGeometryN(i));

        tessellationResults.push_back(tessellatePolygon(poly, trans, symbologyID));
            
        if(!tessellationResults.rbegin()->vertsOut)
        {
            dmess("Warning tessellation failed!");

            tessellationResults.pop_back();
        }
    }
}

Renderable * RenderablePolygon::create( const Polygon * poly,
                                        const dmat4   & trans,
                                        const size_t    symbologyID)
{
    const TessellationResult tess = tessellatePolygon(poly, trans, symbologyID);

    if(!tess.vertsOut) { return NULL ;}

    return createFromTessellations(vector<const TessellationResult>({tess}));
}

Renderable * RenderablePolygon::create( const MultiPolygon  * multiPoly,
                                        const dmat4         & trans,
                                        const size_t          symbologyID)
{
    vector<const TessellationResult> tessellationResults;

    tessellateMultiPolygon(multiPoly, trans, tessellationResults, symbologyID);

    return createFromTessellations(tessellationResults);
}

Renderable * RenderablePolygon::create( const ConstGeosGeomVec & polygons,
                                        const dmat4            & trans,
                                        const size_t             symbologyID)
{
    vector<const TessellationResult> tessellationResults;

    for(const Geometry * geom : polygons)
    {
        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if((poly = dynamic_cast<const Polygon *>(geom)))
        {
            tessellationResults.push_back(tessellatePolygon(poly, trans, symbologyID));
            
            if(!tessellationResults.rbegin()->vertsOut)
            {
                dmess("Warning tessellation failed!");

                tessellationResults.pop_back();
            }

        }
        else if((multiPoly = dynamic_cast<const MultiPolygon *>(geom)))
        {
            dmess("Have a multiPoly!");

            tessellateMultiPolygon(multiPoly, trans, tessellationResults, 0);
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
        }
    }

    return createFromTessellations(tessellationResults);
}

Renderable * RenderablePolygon::create( const vector<pair<const Geometry *, const size_t> >  & polygons, // TODO create a typedef
                                        const dmat4   & trans,
                                        const bool    showProgress)
{
    time_point<system_clock> startTime;
    
    if(showProgress) { startTime = system_clock::now() ;}

    vector<const TessellationResult> tessellationResults;

    for(size_t i = 0; i < polygons.size(); ++i)
    {
        if(showProgress) { doProgress("(6/6) Creating geometry:", i, polygons.size(), startTime) ;}

        const Geometry  * geom        = get<0>(polygons[i]);
        const GLuint      symbologyID = get<1>(polygons[i]);
        
        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if((poly = dynamic_cast<const Polygon *>(geom)))
        {
            tessellationResults.push_back(tessellatePolygon(poly, trans, symbologyID));
            
            if(!tessellationResults.rbegin()->vertsOut)
            {
                dmess("Warning tessellation failed!");

                tessellationResults.pop_back();
            }
        }
        else if((multiPoly = dynamic_cast<const MultiPolygon *>(geom)))
        {
            dmess("Have a multiPoly!");

            //tesselateMultiPolygon(multiPoly, trans, tessellationResults);
            abort();
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
            abort();
        }
    }

    Renderable * ret = createFromTessellations(tessellationResults);

    if(showProgress) { GUI::progress("", 1.0) ;}

    return ret;
}

Renderable * RenderablePolygon::createFromTessellations(const Tessellations & tessellations)
{
    if(!tessellations.size()) { return NULL ;}

    size_t numVerts                 = 0;
    size_t numTriangles             = 0;
    size_t numCounterVertIndices    = 0;
    size_t numCounterVertIndices2   = 0;

    for(const TessellationResult & tess : tessellations)
    {
        numVerts                += tess.numVerts;
        numTriangles            += tess.numTriangles;
        numCounterVertIndices   += tess.counterVertIndices.size();
        numCounterVertIndices2  += tess.counterVertIndices2.size();
    }

    FloatVec verts;

    verts.resize(numVerts * (2 + 1));

    Uint32Vec triangleIndices      (numTriangles * 3);
    Uint32Vec counterVertIndices   (numCounterVertIndices);
    Uint32Vec counterVertIndices2  (numCounterVertIndices2);

    GLfloat * vertsPtr               = &verts[0];
    GLuint  * triangleIndicesPtr     = &triangleIndices[0];
    GLuint  * counterVertIndicesPtr  = &counterVertIndices[0];
    GLuint  * counterVertIndicesPtr2 = &counterVertIndices2[0];

    size_t offset = 0;

    for(size_t i = 0; i < tessellations.size(); ++i)
    {
        const TessellationResult & tess = tessellations[i];

        for(size_t j = 0; j < tess.numVerts; ++j)
        {
            *vertsPtr = tess.vertsOut[j * 2 + 0]; ++vertsPtr;
            *vertsPtr = tess.vertsOut[j * 2 + 1]; ++vertsPtr;

            *vertsPtr = (float(tessellations[i].symbologyID * 4) + 0.5) / 32.0; ++vertsPtr;
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

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);
    
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
                                    tessellations.size() > 1);
}

void RenderablePolygon::render(const mat4 & MVP, const mat4 & MV) const
{
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER,         vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    if(shader->getRenderFill())
    {
        shader->bind(MVP, MV, false);

        shader->enableVertexAttribArray(2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

        shader->enableColorAttribArray(1, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(2 * sizeof(GLuint)));

        glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, NULL);
    }

    if(shader->getRenderOutline())
    {
        shader->bind(MVP, MV, true);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
        
        glDrawElements(GL_LINES, numContourLines, GL_UNSIGNED_INT, NULL);
    }

    glDisable(GL_BLEND);

    //glUseProgram(0);
}
