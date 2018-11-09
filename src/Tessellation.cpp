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

#include <glm/vec4.hpp>
#include <geos/geom/Coordinate.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <GLU/tessellate.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Tessellation.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

Tessellation::Tessellation() { }

Tessellation::Tessellation(const Tessellation&) { abort() ;}
    
Tessellation::~Tessellation()
{
    free(verts);
    free(triangleIndices);
}

Tessellation& Tessellation::operator=(const Tessellation&)
{
    abort();

    return *this;
}

unique_ptr<const Tessellation> Tessellation::tessellatePolygon( const Polygon * poly,
                                                                const dmat4   & trans,
                                                                const size_t    symbologyID,
                                                                const double    height)
{
    Tessellation * ret = new Tessellation;

    ret->symbologyID = symbologyID; // TODO create Tessellation constructor
    ret->height      = height;

    const LineString * ring = poly->getExteriorRing();

    const vector<Coordinate> & coords = *ring->getCoordinatesRO()->toVector();

    if(coords.size() < 4)
    {
        dmess("Bad geometry!");

        return unique_ptr<const Tessellation>(ret);
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

            ret->lineIndices.push_back(i);
            ret->lineIndices.push_back((i + 1) % num);
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

            ret->lineIndices.push_back(i);
            ret->lineIndices.push_back((i + 1) % num);
        }
    }

    ret->counterVertIndices.push_back(0);
    ret->counterVertIndices.push_back(verts.size());

    for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
    {
        const vector<Coordinate> & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad geometry!");

            return unique_ptr<const Tessellation>(ret);
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

                ret->lineIndices.push_back(i + offset);
                ret->lineIndices.push_back(((i + 1) % num) + offset);
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

                ret->lineIndices.push_back(i + offset);
                ret->lineIndices.push_back(((i + 1) % num) + offset);
            }
        }

        ret->counterVertIndices.push_back(verts.size());
    }

    vector<const double *> counterVertPtrs;

    for(size_t i = 0; i < ret->counterVertIndices.size(); ++i) { counterVertPtrs.push_back(&verts[0] + ret->counterVertIndices[i]) ;}

    tessellate( &ret->verts,
                &ret->numVerts,
                &ret->triangleIndices,
                &ret->numTriangles,
                &counterVertPtrs[0],
                &counterVertPtrs[0] + counterVertPtrs.size());

    for(size_t i = 0; i < ret->counterVertIndices.size(); ++i) { ret->counterVertIndices[i] /= 2 ;}

    return unique_ptr<const Tessellation>(ret);
}

void Tessellation::tessellateMultiPolygon(  const MultiPolygon  * multiPoly,
                                            const dmat4         & trans,
                                            Tessellations       & tessellations,
                                            const size_t          symbologyID)
{
    for(size_t i = 0; i < multiPoly->getNumGeometries(); ++i)
    {
        const Polygon * poly = dynamic_cast<const Polygon *>(multiPoly->getGeometryN(i));

        tessellations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID));
            
        if(!(*tessellations.rbegin())->verts)
        {
            dmess("Warning tessellation failed!");

            tessellations.pop_back();
        }
    }
}

bool Tessellation::isEmpty() const { return verts == NULL ;}

/*
const uint32_t * Tessellation::getTriangleIndices() const { return triangleIndices ;}
uint32_t Tessellation::getNumVerts() const { return numVerts ;}
uint32_t Tessellation::getNumTriangles() const { return numTriangles ;}
const Uint32Vec & Tessellation::getCounterVertIndices() const { 
const Uint32Vec & Tessellation::getLineIndices() const;
*/
uint32_t Tessellation::getSymbologyID() const { return symbologyID ;}
double Tessellation::getHeight() const { return height ;}