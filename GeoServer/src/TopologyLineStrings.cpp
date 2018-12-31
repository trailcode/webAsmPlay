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
#include <algorithm>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/prep/PreparedLineString.h>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Util.h>
#include <geoServer/Topology.h>

#include <geoServer/AttributeStatistics.h> // TODO does not belong here

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geos::geom::prep;
using namespace geos::index::quadtree;
using namespace geosUtil;

namespace
{
    size_t counterMyLineString = 0;

    const double epsilon = 0.0000001;

    class MyLineString
    {
    public:

        MyLineString(AttributedLineString * ls) :   ls      (ls),
                                                    noSplits(false),
                                                    length  (get<1>(*ls)->getLength())
        {
            ++counterMyLineString;
        }

        ~MyLineString()
        { 
            --counterMyLineString;

            delete pls;
        }

        const LineString * getLS() const { return get<1>(*ls) ;}

        void save(vector<AttributedLineString> & out) const
        {
            if(splits.size())
            {
                for(auto i : splits)
                {
                    out.push_back(AttributedLineString(get<0>(*ls), i));
                }
            }
            else if(!getLS())
            {
                dmess("This!");    
            }
            else
            {
                out.push_back(*ls);
            }
        }

        PreparedLineString * getPLS()
        {
            if(pls) { return pls ;}

            return pls = new PreparedLineString(getLS());
        }

        void deleteOrigionalGeom()
        {
            //GeometryFactory::getDefaultInstance()->destroyGeometry(get<1>(*ls));
            //delete get<1>(*ls);

            //get<1>(*ls) = NULL;
        }

        list<LineString *> splits; // TODO try to put the below ls in splits initially. 

        AttributedLineString * ls;

        PreparedLineString * pls = NULL;

        bool noSplits;

        float length;
    };

    size_t numSplits;

    size_t lastNumSplits = 0;

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

    inline bool intersects(const Geometry * A, const Geometry * B)
    {
        try
        {
            if(A->intersects(B))
            {
                //if(A->overlaps(B) || B->overlaps(A))
                if(false && A->overlaps(B) || B->overlaps(A))
                {
                    dmess("jfskdfd");

                    return false;
                }

                //dmess("jjjjjj");

                return true;
            }

            return false;
        }

        catch(...)
        {
            dmess("Warning intersection check error!");

            return false;
        }
    }

    // TODO Memory leaks!
    inline bool doSplitting(MyLineString * B, const LineString * ls, const LineString * curr)
    {
        Geometry * g = scopedGeosGeometry(ls->difference(curr));

        lastNumSplits = 0;

        bool didSplit = false;

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

                    if(splitLS->getLength() < epsilon * 2.0) { continue ;}

                    ++numSplits;

                    ++lastNumSplits;

                    didSplit = true;

                    B->splits.push_front(dynamic_cast<LineString *>(splitLS->clone()));
                }

                break;
            }
            default:

                break;
        }

        if(didSplit)
        {
            B->deleteOrigionalGeom();
        }

        return didSplit;
    }

    inline bool samePoint(const geos::geom::Point * A, const geos::geom::Point * B)
    {
        return A->getX() == B->getX() && A->getY() == B->getY();
    }

    inline bool endPointsTouch2D(const geos::geom::LineString * A, const geos::geom::LineString * B)
    {
        const Coordinate & P1 = A->getCoordinatesRO()->getAt(0);
        const Coordinate & P2 = B->getCoordinatesRO()->getAt(0);
        const Coordinate & P3 = A->getCoordinatesRO()->getAt(A->getCoordinatesRO()->getSize() - 1);
        const Coordinate & P4 = B->getCoordinatesRO()->getAt(B->getCoordinatesRO()->getSize() - 1);

        return P1.equals2D(P2) || P3.equals2D(P4) || P1.equals2D(P4) || P3.equals2D(P2);
    }
}

vector<AttributedLineString> _breakLineStrings(vector<AttributedLineString> & lineStrings, vector<AttributedLineString> & nonSplitting)
{
    dmess("start topology::breakLineStrings " << lineStrings.size() << " MyLineString " << counterMyLineString);

    vector<AttributedLineString> ret;

    Quadtree tree;

    vector<MyLineString *> myLineStrings;

    for(auto & ls : lineStrings)
    {
        MyLineString * myLS = new MyLineString(&ls);

        tree.insert(get<1>(ls)->getEnvelopeInternal(), (void *)myLS);

        myLineStrings.push_back(myLS);
    }

    sort(myLineStrings.begin(), myLineStrings.end(), [](const MyLineString * lhs, const MyLineString * rhs)
    {
        return lhs->length < rhs->length;
    });

    dmess("Tree depth " << tree.depth());

    numSplits = 0;

    size_t maxNumSplits = 5;

    size_t counter = 0;

    for(auto ls : myLineStrings)
    {
        if(!(counter % 300)) { dmess("myLineStrings " << myLineStrings.size() << " " << counter) ;}

        counter++;

        //if(counter > 2500) { break ;}
        //if(counter > 1000) { break ;}

        if(!ls->getLS()) { continue ;}

        //*
        if(ls->splits.size()) { continue ;}

        if(ls->splits.size() > maxNumSplits)
        {
            //dmess("skip here!");

            continue;
        }
        //*/

        vector< void * > query;

        tree.query(ls->getLS()->getEnvelopeInternal(), query);

        //dmess("query " << query.size());

        //vector<MyLineString *> intersecting;

        auto curr    = ls->getLS();
        auto currPLS = ls->getPLS();

        //unique_ptr<LineString> currExtended(extendEnds(curr));

        bool didSplit = false;
        bool didSkip  = false;

        for(auto _B : query)
        {
            MyLineString * B = (MyLineString *)_B;

            if(B->getLS() == curr) { continue ;}
            
            //if(false && B->splits.size() > maxNumSplits)
            if(B->splits.size() > maxNumSplits)
            {
                didSkip = true;

                continue;
            }
            
            if(B->splits.size())
            {
                auto it = B->splits.begin();

                //bool gotIntersect = false;

                while (it != B->splits.end())
                {
                    if(endPointsTouch2D(*it, curr))
                    {
                        ++it;

                        continue;
                    }

                    //if(intersects(*it, currExtended.get()))
                    //if(intersects(*it, curr))
                    if(currPLS->intersects(*it))
                    {
                        //didSplit |= doSplitting(B, *it, currExtended);
                        didSplit |= doSplitting(B, *it, curr);

                        //gotIntersect = true;

                        it = B->splits.erase(it);
                    }
                    else { ++it ;}
                }

                //if(gotIntersect) { intersecting.push_back(B) ;}

                continue;
            }

            if(!B->getLS()) { continue ;}

            if(endPointsTouch2D(B->getLS(), curr)) { continue ;}

            //if(!intersects(B->getLS(), currExtended.get())) { continue ;}
            //if(!intersects(B->getLS(), curr)) { continue ;}
            if(!currPLS->intersects(B->getLS())) { continue ;}

            //intersecting.push_back(B);

            //didSplit |= doSplitting(B, B->getLS(), currExtended);
            didSplit |= doSplitting(B, B->getLS(), curr);
        }
        
        if(!didSplit && !didSkip)
        {
            //dmess("Done!");

            ls->noSplits = true;
        }
    }

    for(auto ls : myLineStrings)
    {
        if(ls->noSplits) { ls->save(nonSplitting) ;}

        else { ls->save(ret) ;}

        delete ls;
    }

    dmess("end topology::breakLineStrings " << ret.size() << " nonSplitting " << nonSplitting.size() << " num splits: " << numSplits);

    return ret;
}

void topology::breakLineStrings(vector<AttributedLineString> & lineStrings)
{
    AttributeStatistics::addLineStrings(lineStrings);

    vector<AttributedLineString> curr; // = lineStrings;

    size_t counter = 0;

    for(auto & i : lineStrings)
    {
        //if(get<0>(i)->hasStringKey("footway") || get<0>(i)->hasStringKeyValue("footway", "sidewalk") || get<0>(i)->hasStringKeyValue("highway", "footway")) { curr.push_back(i) ;}
        //if(get<0>(i)->hasStringKey("footway"))
        //if( get<0>(i)->hasStringKeyValue("highway", "residential") || get<0>(i)->hasStringKeyValue("highway", "service")) { curr.push_back(i) ;}
        //if(get<0>(i)->hasStringKeyValue("highway", "service")) { curr.push_back(i) ;}
        //if(get<0>(i)->hasStringKeyValue("highway", "residential") || get<0>(i)->hasStringKeyValue("highway", "motorway"))
        //if(get<0>(i)->hasStringKeyValue("highway", "motorway"))
        //if(get<0>(i)->hasStringKeyValue("highway", "motorway"))
        {
            curr.push_back(i);

            ++counter;

            //if(counter > 20000) { break ;}
        }
    }

    //return curr;

    vector<AttributedLineString> nonSplitting;

    do
    {
        // TODO do we have a memory leak here?
        curr = _breakLineStrings(curr, nonSplitting);

        for(auto & j : nonSplitting)
        {
            //delete get<1>(j);
        }
    }
    while(numSplits);

    lineStrings.clear();

    lineStrings.insert(lineStrings.end(), nonSplitting.begin(), nonSplitting.end());
}