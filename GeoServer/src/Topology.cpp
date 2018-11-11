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

#include <memory>
#include <list>
#include <glm/vec2.hpp>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Util.h>
#include <geoServer/Topology.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geos::index::quadtree;
using namespace geosUtil;

namespace
{
    class MyLineString
    {
    public:

        MyLineString(AttributedLineString * ls) : ls(ls) {}

        const LineString * getLS() const { return get<1>(*ls) ;}

        list<const LineString *> splits; // TODO try to put the below ls in splits initially. 

        AttributedLineString * ls;
    };

    size_t numSplits;

    size_t lastNumSplits = 0;

    const double epsilon = 0.00001;

    inline LineString * extendEnds(const LineString * ls)
    {
        const auto coords = *ls->getCoordinatesRO()->toVector();

        const dvec2 P1 = __(coords[0]);
        const dvec2 P2 = __(coords[1]);
        const dvec2 P3 = __(coords[coords.size() - 1]);
        const dvec2 P4 = __(coords[coords.size() - 2]);

        const dvec2 start = (normalize(P1 - P2) * epsilon) + P1;
        const dvec2 end   = (normalize(P3 - P4) * epsilon) + P3;

        vector<Coordinate> * newCoords = new vector<Coordinate>();

        newCoords->reserve(coords.size() + 2);

        newCoords->push_back(___(start));

        newCoords->insert(newCoords->end(), coords.begin(), coords.end());

        newCoords->push_back(___(end));

        return GeometryFactory::getDefaultInstance()->createLineString(new CoordinateArraySequence(newCoords, 2));
    }

    // TODO Memory leaks!

    inline void doSplitting(MyLineString * B, const LineString * ls, const unique_ptr<LineString> & curr)
    {
        Geometry * g = ls->difference(curr.get());

        lastNumSplits = 0;

        switch(g->getGeometryTypeId())
        {
            case GEOS_MULTILINESTRING:
            case GEOS_GEOMETRYCOLLECTION:
            {
                const GeometryCollection * gc = dynamic_cast<const GeometryCollection *>(g);

                for(size_t i = 0; i < gc->getNumGeometries(); ++i)
                {
                    const Geometry * g = gc->getGeometryN(i);

                    if(g->getGeometryTypeId() != GEOS_LINESTRING)
                    {
                        dmess("Warning! Not LS: " << g->getGeometryType());

                        continue;
                    }

                    const LineString * splitLS = dynamic_cast<const LineString *>(g);

                    if(splitLS->getLength() < epsilon) { continue ;}

                    ++numSplits;

                    ++lastNumSplits;

                    B->splits.push_front(splitLS);
                }

                break;
            }
            default:

                break;
        }
    }
}

vector<AttributedLineString> _breakLineStrings(vector<AttributedLineString> & lineStrings)
{
    dmess("start topology::breakLineStrings " << lineStrings.size());

    vector<AttributedLineString> ret;

    Quadtree tree;

    vector<MyLineString *> myLineStrings;

    for(auto & ls : lineStrings)
    {
        MyLineString * myLS = new MyLineString(&ls);

        tree.insert(get<1>(ls)->getEnvelopeInternal(), (void *)myLS);

        myLineStrings.push_back(myLS);
    }

    dmess("Tree depth " << tree.depth());

    numSplits = 0;

    for(auto ls : myLineStrings)
    {
        vector< void * > query;

        tree.query(ls->getLS()->getEnvelopeInternal(), query);

        //dmess("query " << query.size());

        vector<MyLineString *> intersecting;

        auto curr = ls->getLS();

        unique_ptr<LineString> currExtended(extendEnds(curr));

        for(auto _B : query)
        {
            MyLineString * B = (MyLineString *)_B;

            if(B->getLS() == curr) { continue ;}

            if(B->splits.size())
            {
                auto it = B->splits.begin();

                bool gotIntersect = false;

                while (it != B->splits.end())
                {
                    if(endPointsTouch(*it, curr))
                    {
                        ++it;

                        continue;
                    }

                    if((*it)->intersects(currExtended.get()))
                    //if((*it)->intersects(curr) || (*it)->touches(curr))
                    {
                        doSplitting(B, *it, currExtended);

                        gotIntersect = true;

                        it = B->splits.erase(it);
                    }
                    else { ++it ;}
                }

                if(gotIntersect) { intersecting.push_back(B) ;}

                continue;
            }

            if(endPointsTouch(B->getLS(), curr)) { continue ;}

            if(!B->getLS()->intersects(currExtended.get())) { continue ;}
            //if(!B->getLS()->intersects(curr) && !B->getLS()->touches(curr)) { continue ;}
            
            intersecting.push_back(B);

            doSplitting(B, B->getLS(), currExtended);
        }
        
        /*
        for(auto B : intersecting)
        {
            //void doSplitting(MyLineString * B, const LineString * ls, const LineString * curr)
            if(ls->splits.size())
            {
                auto it = ls->splits.begin();

                while (it != ls->splits.end()) // TODO code duplication
                {
                    if((*it)->intersects(curr))
                    {
                        doSplitting(ls, *it, B->getLS());

                        //dmess("lastNumSplits " << lastNumSplits);

                        it = ls->splits.erase(it);
                    }
                    else { ++it ;}
                }
            }
            else
            {
                doSplitting(ls, ls->getLS(), B->getLS());
            }
        }
        //*/
    }

    for(auto ls : myLineStrings)
    {
        if(ls->splits.size())
        {
            for(auto i : ls->splits)
            {
                Attributes * a = ls->ls->first;

                ret.push_back(AttributedLineString(get<0>(*(ls->ls)), i));
            }
        }
        else
        {
            ret.push_back(*(ls->ls));
        }
    }

    dmess("end topology::breakLineStrings " << ret.size() << " num splits: " << numSplits);

    return ret;
}

vector<AttributedLineString> topology::breakLineStrings(vector<AttributedLineString> & lineStrings)
{
    vector<AttributedLineString> curr = lineStrings;

    size_t i = 0;

    do
    {
        curr = _breakLineStrings(curr);

        if(++i > 4) { break ;}
    }

    while(numSplits);

    return curr;
}