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
#include <geos/geom/Polygon.h>
#include <geos/geom/prep/PreparedPolygon.h>
#include <geos/index/quadtree/Quadtree.h>
#include <geos/io/WKTWriter.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Attributes.h>
#include <geoServer/Topology.h>

using namespace std;
using namespace geos::geom;
using namespace geos::geom::prep;
using namespace geos::index::quadtree;

namespace
{
    class MyPolygon
    {
    public:
        MyPolygon(  Attributes		 * attrs,
                    const Polygon    * poly,
                    const double       area) :  attrs         (attrs),
                                                poly          (poly),
                                                polyPrepaired (poly),
                                                area          (area) {}

        uint32_t getID() const { return attrs->m_uints32.find("ID")->second ;}

        const Polygon * poly;

        const PreparedPolygon polyPrepaired;

        const double area;

        Attributes * attrs;
    };

    size_t lastID = 0; // TODO find a better way to do IDs.
}

void topology::discoverTopologicalRelations(vector<AttributedPoligonalArea> & polygons)
{
    dmess("start topology::discoverTopologicalRelations");

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

        MyPolygon * parent = NULL;

        for(size_t j = 0; j < query.size(); ++j)
        {
            MyPolygon * potentalParent = (MyPolygon *)query[j];

            if(potentalParent->poly == potentialChild) { continue ;}

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
	
		//parent->attrs->multiUints32s["allChildIDs"].push_back(attrs(i)->uints32["ID"]);
		parent->attrs->m_multiUints32s["childIDs"].push_back(attrs(i)->m_uints32["ID"]);
    }

	/*
	for (const auto& i : polygons)
	{
		const vector<uint32_t> & allChildIDs = attrs(i)->multiUints32s["allChildIDs"];

		vector<uint32_t> & childIDs = attrs(i)->multiUints32s["childIDs"];

		for (const auto childID : allChildIDs)
		{
			auto child = polygons[childID];

			for (const auto ID : attrs(child)->multiUints32s["allChildIDs"])
			{
				if (ID == childID) { goto next; }
			}

			childIDs.push_back(childID);
		}

	next:;
	}

	for (const auto& i : polygons)
	{
		if (!attrs(i)->multiUints32s["allChildIDs"].size()) { continue; }

		dmess(attrs(i)->multiUints32s["allChildIDs"].size() << " " << attrs(i)->multiUints32s["childIDs"].size());
	}
	*/

	//exit(0);

    dmess("done topology::discoverTopologicalRelations");
}

void topology::cutPolygonHoles(vector<AttributedPoligonalArea>& polygons)
{
	//return;

	dmess("start cutPolygonHoles");

	size_t c = 0;

	const GeometryFactory* geomFactory = GeometryFactory::getDefaultInstance();

	size_t cc = 0;

	for (auto& i : polygons)
	{
		const vector<uint32_t> & childIDs = attrs(i)->m_multiUints32s["childIDs"];

		++cc;

		if (childIDs.size()) { dmess(" " << cc << " " << polygons.size()); }

		for (const auto childID : childIDs)
		{
			Polygon* P = poly(i);

			if (P == poly(polygons[childID]))
			{
				dmess("Here!");

				continue;
			}

			Geometry* newPoly = P->difference(poly(polygons[childID]));

			if (!dynamic_cast<Polygon*>(newPoly))
			{
				dmess("Not poly! " << newPoly->getGeometryType());

				geos::io::WKTWriter w;

				char buf[1024];

				sprintf(buf, "C:/Temp/p_%i.wkt", ++c);

				ofstream out(buf);

				//w.write(*newPoly, out);
				out << w.write(newPoly);

				out.close();

				geomFactory->destroyGeometry(newPoly);

				continue;
			}
			
			//dmess("P " << P->getNumInteriorRing() << " " << dynamic_cast<Polygon*>(newPoly)->getNumInteriorRing());

			geomFactory->destroyGeometry(P);

			poly(i) = dynamic_cast<Polygon*>(newPoly);
		}
	}

	dmess("End cutPolygonHoles");
}