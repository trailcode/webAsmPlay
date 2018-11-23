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

#include <algorithm>
#include <geos/geom/Polygon.h>
#include <geos/geom/prep/PreparedPolygon.h>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/Debug.h>
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
        MyPolygon(  const Attributes * attrs,
                    const Polygon    * poly,
                    const double       area) :  attrs        (attrs),
                                                poly         (poly),
                                                polyPrepaired(poly),
                                                area         (area) {}

        uint32_t getID() const { return attrs->uints32.find("ID")->second ;}

        const Polygon * poly;

        const PreparedPolygon polyPrepaired;

        const double area;

        const Attributes * attrs;
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

        attrs(i)->uints32["ID"] = ++lastID; 
    }

    dmess("quadTree depth " << quadTree.depth() << " num polys: " << polygons.size());

    vector< void * > query;

    for(auto & i : polygons)
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

            if(!potentalParent->polyPrepaired.contains(potentialChild)) { continue ;}

            if(potentalParent->area > minArea) { continue ;}

            minArea = potentalParent->area;

            parent = potentalParent;
        }

        if(parent) { attrs(i)->uints32["parentID"] = parent->getID() ;}
    }

    dmess("done topology::discoverTopologicalRelations");
}