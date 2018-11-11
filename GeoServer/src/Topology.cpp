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
#include <geos/geom/LineString.h>
#include <GEOS/geom/GeometryCollection.h>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/Debug.h>
#include <geoServer/Topology.h>

using namespace std;
using namespace geos::geom;
using namespace geos::index::quadtree;

namespace
{
    class MyLineString
    {
    public:

        MyLineString(AttributedLineString * ls) : ls(ls) {}

        const LineString * getLS() const { return get<1>(*ls) ;}

        list<const LineString *> splits;

        AttributedLineString * ls;
    };

    void doSplitting(MyLineString * B, const LineString * ls, const LineString * curr)
    {
        //Geometry * g = B->getLS()->difference(curr);
        Geometry * g = ls->difference(curr);

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

                    B->splits.push_front(dynamic_cast<const LineString *>(g));
                }

                break;
            }
            default:

                break;
        }
    }
}

vector<AttributedLineString> topology::breakLineStrings(vector<AttributedLineString> & lineStrings)
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

    for(auto ls : myLineStrings)
    {
        vector< void * > query;

        tree.query(ls->getLS()->getEnvelopeInternal(), query);

        //dmess("query " << query.size());

        vector<MyLineString *> intersecting;

        auto curr = ls->getLS();

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
                    if((*it)->intersects(curr))
                    {
                        doSplitting(B, *it, curr);

                        gotIntersect = true;

                        it = B->splits.erase(it);
                    }
                    else
                    {
                        ++it;  // go to next
                    }
                }

                if(gotIntersect) { intersecting.push_back(B) ;}

                continue;
            }

            if(!B->getLS()->intersects(curr)) { continue ;}
            
            intersecting.push_back(B);

            doSplitting(B, B->getLS(), curr);
        }
        
        //dmess("intersecting " << intersecting.size());
    }

    for(auto ls : myLineStrings)
    {
        // typedef std::pair<Attributes *, geos::geom::LineString *> AttributedLineString;
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

    dmess("end topology::breakLineStrings " << ret.size());

    return ret;
}
