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

#include <fstream>
#include <algorithm>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/prep/PreparedPolygon.h>
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geos/io/WKTWriter.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <geoServer/Topology.h>

using namespace std;
using namespace geos::geom;
using namespace geos::geom::prep;
using namespace geos::index::quadtree;
using namespace geos::operation::geounion;
using namespace geosUtil;

namespace
{
    class MyPolygon
    {
    public:
        MyPolygon(  Attributes		 * attrs,
                    Polygon    * poly,
                    const double       area) :  attrs         (attrs),
                                                geom          (poly),
                                                polyPrepaired (poly),
                                                area          (area) {}

        uint32_t getID() const { return attrs->m_uints32.find("ID")->second ;}

        Geometry * geom;

        const PreparedPolygon polyPrepaired;

        const double area;

        Attributes * attrs;

		vector<const AttributedPoligonalArea *> children;
    };

    size_t lastID = 0; // TODO find a better way to do IDs.
}

vector<AttributedPoligonalArea> topology::discoverTopologicalRelations(vector<AttributedPoligonalArea> & polygons)
{
    dmess("start topology::discoverTopologicalRelations");

	// Add AOI polygon.
	//makeBox(get<2>(m_bounds), get<3>(m_bounds), get<0>(m_bounds), get<1>(m_bounds)).release());
	
	//polygons

    // Sort the polygons by their area. Smallest first.
    sort(polygons.begin(), polygons.end(), [](const AttributedPoligonalArea & lhs,
                                              const AttributedPoligonalArea & rhs) { return get<2>(lhs) < get<2>(rhs) ;});

    Quadtree quadTree;

    vector<unique_ptr<MyPolygon> > myPolygons;

    for(const auto & i : polygons)
    {
        poly(i)->normalize();
        
        MyPolygon * myPoly = new MyPolygon(attrs(i), poly(i), area(i));

        myPolygons.push_back(unique_ptr<MyPolygon>(myPoly));
        
        quadTree.insert(poly(i)->getEnvelopeInternal(), myPoly);

        attrs(i)->m_uints32["ID"] = (uint32_t)++lastID; 
    }

    dmess("quadTree depth " << quadTree.depth() << " num polys: " << polygons.size());

    vector< void * > query;

    for(const auto & i : polygons)
    {
        const Polygon * potentialChild = poly(i);

        query.clear();
    
        quadTree.query(poly(i)->getEnvelopeInternal(), query);

        double minArea = numeric_limits<double>::max();

        MyPolygon * parent = nullptr;

        for(size_t j = 0; j < query.size(); ++j)
        {
            MyPolygon * potentalParent = (MyPolygon *)query[j];

			if (potentalParent->geom == potentialChild) { continue; }

			try
			{
				if (!potentalParent->polyPrepaired.contains(potentialChild)) { continue; }
			}
			catch (...)
			{
				dmess("Here!");

				continue;
			}

            if(potentalParent->area > minArea) { continue ;}

            minArea = potentalParent->area;

            parent = potentalParent;
        }

		if (!parent) { continue; }

		attrs(i)->m_uints32["parentID"] = parent->getID();
	
		parent->children.push_back(&i);
    }

	dmess("Done find children.");

	return polygons;

	int c = 0;
	
	for (auto& i : myPolygons)
	{ 
		//dmess("i->children " << i->children.size() << " " << c++);

		++c;

		if (false && c == 3568)
		{
			vector<const Geometry*> cc;
			for (const auto& child : i->children) { cc.push_back(poly(*child)); }
			geosUtil::writeGeoJsonFile("children.geojson", cc);
			dmess("done write");
		}

		if (!i->children.size()) { continue; }

		//dmess("c " << c << " " << myPolygons.size() << " " << i->children.size());

		vector<Polygon*> children;
		
		for (const auto& child : i->children) { children.push_back(poly(*child)); }

		Geometry * unionedChildren = CascadedPolygonUnion::Union(&children);

		i->geom = i->geom->difference(unionedChildren);
	}

	vector<AttributedPoligonalArea> ret;

	c = 0;

	for (auto& i : myPolygons)
	{
		switch (i->geom->getGeometryTypeId())
		{
		case GEOS_POLYGON: ret.push_back(AttributedPoligonalArea(i->attrs, geosPolygon(i->geom->buffer(0)), i->area)); break;
		case GEOS_GEOMETRYCOLLECTION:
		case GEOS_MULTIPOLYGON:
			{
				/*
				++c;

				char buf[1024];

				sprintf(buf, "outa/mult_%i.geojson", c);

				geosUtil::writeGeoJsonFile(buf, i->geom);
				*/

				GeometryCollection* collection = dynamic_cast<GeometryCollection*>(i->geom);

				for (size_t j = 0; j < collection->getNumGeometries(); ++j)
				{
					const Polygon * polygon = geosPolygonConst(collection->getGeometryN(j));

					if (!polygon)
					{
						dmess("Not polygon! " << collection->getGeometryN(j)->getGeometryType());

						continue;
					}

					ret.push_back(AttributedPoligonalArea(i->attrs, geosPolygon(polygon->buffer(0)), polygon->getArea()));
				}
				break;
			}

		default:
			dmess("Unsupported geometry type: " << i->geom->getGeometryType());
		}		
	}

	dmess("done topology::discoverTopologicalRelations");

	return ret;
}

