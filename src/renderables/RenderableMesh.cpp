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
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/renderables/RenderableMesh.h>

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;

Renderable * RenderableMesh::create( const ColoredExtrudedGeometryVec & polygons,
                                     const dmat4                      & trans,
                                     const bool                         showProgress)
{
    time_point<system_clock> startTime;
    
    if(showProgress) { startTime = system_clock::now() ;}

    vector<const Tessellation> tessellations;

    for(size_t i = 0; i < polygons.size(); ++i)
    {
        if(showProgress) { doProgress("(6/6) Creating geometry:", i, polygons.size(), startTime) ;}

        const Geometry  * geom        = get<0>(polygons[i]);
        const GLuint      symbologyID = get<1>(polygons[i]);
        const double      height      = get<2>(polygons[i]);
        
        const Polygon      * poly;
        const MultiPolygon * multiPoly;

        if((poly = dynamic_cast<const Polygon *>(geom)))
        {
            tessellations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID, height));
            
            if(!tessellations.rbegin()->vertsOut)
            {
                dmess("Warning tessellation failed!");

                tessellations.pop_back();
            }
        }
        else if((multiPoly = dynamic_cast<const MultiPolygon *>(geom)))
        {
            dmess("Have a multiPoly!");

            //tesselateMultiPolygon(multiPoly, trans, tessellations);
            abort();
        }
        else
        {
            dmess("Warning not a polygon or multi-polygon.");
            abort();
        }
    }

    Renderable * ret = createFromTessellations(tessellations);

    if(showProgress) { GUI::progress("", 1.0) ;}

    return ret;
}

Renderable * RenderableMesh::createFromTessellations(const Tessellations & tessellations)
{
    return NULL;
}

RenderableMesh::~RenderableMesh()
{
    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers     (1, &vbo));
    GL_CHECK(glDeleteBuffers     (1, &ebo));
    GL_CHECK(glDeleteBuffers     (1, &ebo2));
}

RenderableMesh::RenderableMesh( const GLuint      vao,
                                const GLuint      ebo,
                                const GLuint      ebo2,
                                const GLuint      vbo,
                                const int         numTriangles,
                                const Uint32Vec & counterVertIndices,
                                const size_t      numContourLines) : Renderable          (true),
                                                                     vao                 (vao),
                                                                     ebo                 (ebo),
                                                                     ebo2                (ebo2),
                                                                     vbo                 (vbo),
                                                                     numTriangles        (numTriangles),
                                                                     counterVertIndices  (counterVertIndices),
                                                                     numContourLines     (numContourLines)
{

}

void RenderableMesh::render(const mat4 & MVP, const mat4 & MV) const
{

}