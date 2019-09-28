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

#ifdef WIN32
#define Polygon Polygon_Custom // Prevent Polygon ambiguity
#include <windows.h>
#undef Polygon
#endif

#include <GLU/tessellate.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Tessellation.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

Tessellation::Tessellation(const size_t symbologyID,
                           const double height,
                           const double minHeight) : m_symbologyID(uint32_t	(symbologyID)),
                                                     m_height				(height),
                                                     m_minHeight			(minHeight) {}
    
Tessellation::~Tessellation()
{
    free(m_verts);
    free(m_triangleIndices);
}

template<typename CoordVec>
void Tessellation::tessellateRing(	const dmat4		& trans,
									const CoordVec	& inVerts,
									Tessellation	* tess,
									vector<double>	& outVerts,
									const bool		  isOuter)
{
	const size_t num = inVerts.size() - 1;

	const size_t offset = outVerts.size() / 2;

	if(trans == dmat4(1.0))
	{
		for(size_t i = 0; i < num; ++i)
		{
			append2d(outVerts, __(inVerts[i]));

			append2ui(tess->m_lineIndices, i + offset, ((i + 1) % num) + offset);
		}
	}
	else
	{
		for(size_t i = 0; i < num; ++i)
		{
			append2d(outVerts, trans * dvec4(__(inVerts[i]), 0, 1));

			append2ui(tess->m_lineIndices, i + offset, ((i + 1) % num) + offset);
		}
	}

	if(!isOuter)
	{
		tess->m_counterVertIndices.push_back(uint32_t(outVerts.size()));
	}
	else
	{
		tess->m_counterVertIndices.push_back(0);
		tess->m_counterVertIndices.push_back(uint32_t(outVerts.size()));
	}
}

Tessellation::ConstPtr Tessellation::tessellatePolygon(	const Polygon * poly,
														const dmat4   & trans,
														const size_t    symbologyID,
														const double    height,
														const double    minHeight)
{
    auto ret = new Tessellation(symbologyID, height, minHeight);

    const auto & coords = *poly->getExteriorRing()->getCoordinatesRO()->toVector();
	
    if(coords.size() < 4)
    {
        dmess("Bad geometry!");

        return unique_ptr<const Tessellation>(ret);
    }

	vector<double> verts;

	tessellateRing(trans, coords, ret, verts, true);

    for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
    {
        const auto & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad geometry!");

            return unique_ptr<const Tessellation>(ret);
        }

		tessellateRing(trans, coords, ret, verts, false);
    }

    vector<const double *> counterVertPtrs;

    for(size_t i = 0; i < ret->m_counterVertIndices.size(); ++i) { counterVertPtrs.push_back(&verts[0] + ret->m_counterVertIndices[i]) ;}

    tessellate( &ret->m_verts,
                &ret->m_numVerts,
                &ret->m_triangleIndices,
                &ret->m_numTriangles,
                &counterVertPtrs[0],
                &counterVertPtrs[0] + counterVertPtrs.size());

    for(size_t i = 0; i < ret->m_counterVertIndices.size(); ++i) { ret->m_counterVertIndices[i] /= 2 ;}

    return unique_ptr<const Tessellation>(ret);
}

Tessellation::ConstPtr Tessellation::tessellatePolygon(	const boostGeom::Polygon	& poly,
														const dmat4					& trans,
														const size_t				  symbologyID,
														const double				  height,
														const double				  minHeight)
{
	auto ret = new Tessellation(symbologyID, height, minHeight);

	if(poly.outer().size() < 4)
	{
		dmess("Bad geometry!");

        return unique_ptr<const Tessellation>(ret);
	}

	vector<double> verts;

	tessellateRing(trans, poly.outer(), ret, verts, true);

	vector<const double *> counterVertPtrs;

    for(size_t i = 0; i < ret->m_counterVertIndices.size(); ++i) { counterVertPtrs.push_back(&verts[0] + ret->m_counterVertIndices[i]) ;}

    tessellate( &ret->m_verts,
                &ret->m_numVerts,
                &ret->m_triangleIndices,
                &ret->m_numTriangles,
                &counterVertPtrs[0],
                &counterVertPtrs[0] + counterVertPtrs.size());

    for(size_t i = 0; i < ret->m_counterVertIndices.size(); ++i) { ret->m_counterVertIndices[i] /= 2 ;}

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
            
        if(!(*tessellations.rbegin())->m_verts)
        {
            dmess("Warning tessellation failed!");

            tessellations.pop_back();
        }
    }
}

void Tessellation::tessellateMultiPolygon(	const boostGeom::MultiPolygon	& multiPoly,
											const dmat4						& trans,
											Tessellations					& tessellations,
											const size_t					  symbologyID)
{
	for(const auto & poly : multiPoly)
	{
		tessellations.push_back(Tessellation::tessellatePolygon(poly, trans, symbologyID));
            
        if(!(*tessellations.rbegin())->m_verts)
        {
            dmess("Warning tessellation failed!");

            tessellations.pop_back();
        }
	}
}

bool Tessellation::isEmpty() const { return m_verts == nullptr ;}

uint32_t Tessellation::getSymbologyID() const { return m_symbologyID ;}

double Tessellation::getHeight() const { return m_height ;}